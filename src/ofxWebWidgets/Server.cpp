#include "Server.h"

namespace ofxWebWidgets {
	static const char* CONTENT_TYPE = "Content-Type";

	struct CustomProcessor {
		bool active = false;
		stringstream data;
	};

	class connection_info {
		static size_t id;
	public:
		connection_info() {
			conn_id = ++id;
		}

		map<string, string> fields;
		map<string, FILE*> file_fields;
		map<string, string> file_to_path_index;
		map<string, string> file_to_key_index;
		Request::Method requestMethod;
		bool connection_complete;
		struct MHD_PostProcessor *postprocessor;
		int conn_id;
		char new_content_type[1024];
		CustomProcessor customProcessor;
	};

	size_t connection_info::id = 0;

#pragma mark public
	//-----------
	Server & Server::X() {
		static Server instance;
		return instance;
	}

	//-----------
	Server::~Server() {
		this->stop();
	}

	//-----------
	void Server::start(const Parameters & parameters) {
		this->parameters = parameters;

		this->stop();

		this->daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION
			, this->parameters.port
			, NULL
			, NULL
			, &handleConnection
			, NULL
			, MHD_OPTION_NOTIFY_COMPLETED
			, &request_completed
			, NULL
			, MHD_OPTION_END);
	}

	//-----------
	void Server::stop() {
		if (this->daemon) {
			MHD_stop_daemon(daemon);
			this->daemon = NULL;
		}
	}

	//-----------
	bool Server::isRunning() const {
		return this->daemon != NULL;
	}


	//-----------
	void Server::addRequestHandler(RequestHandler * requestHandler) {
		//remove in-case somewhere else we have already added this handler
		this->removeRequestHandler(requestHandler);
		this->requestHandlers.insert(requestHandler);
	}

	//-----------
	void Server::removeRequestHandler(RequestHandler * requestHandler) {
		auto findRequesthandler = this->requestHandlers.find(requestHandler);
		if (findRequesthandler != this->requestHandlers.end()) {
			this->requestHandlers.erase(findRequesthandler);
		}
	}

	//-----------
	std::set<RequestHandler *> & Server::getRequestHandlers() {
		return this->requestHandlers;
	}


	//-----------
	const std::set<RequestHandler *> & Server::getRequestHandlers() const {
		return this->requestHandlers;
	}

	//-----------
	const Server::Parameters & Server::getParameters() const {
		return this->parameters;
	}

#pragma mark private
	//-----------
	Server::Server() {
		ofAddListener(ofEvents().exit, this, &Server::onExit);
	}

	//-----------
	int Server::handleConnection(void * cls
		, struct MHD_Connection * connection
		, const char * url
		, const char * method
		, const char * version
		, const char * upload_data
		, size_t * upload_data_size
		, void ** con_cls) {

		string strmethod = method;

		connection_info  * con_info;

		auto & instance = Server::X();
		const auto & parameters = instance.getParameters();

		// to process post we need several iterations, first we set a connection info structure
		// and return MHD_YES, that will make the server call us again
		if (NULL == *con_cls) {
			con_info = new connection_info;

			instance.activeClientCount++;

			if (instance.activeClientCount >= parameters.maximumClients) {
				return Server::send_error(connection, 503);
			}

			// super ugly hack to manage poco multi part post connections as it sets boundary between "" and
			// libmicrohttpd doesn't seem to support that
			string contentType;
			if (MHD_lookup_connection_value(connection, MHD_HEADER_KIND, CONTENT_TYPE) != NULL)
				contentType = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, CONTENT_TYPE);
			if (contentType.size() > 31 && contentType.substr(0, 31) == "multipart/form-data; boundary=\"") {
				contentType = "multipart/form-data; boundary=" + contentType.substr(31, contentType.size() - 32);
				ofLogVerbose("ofxWebWidgets::Server") << "changing content type: " << contentType << endl;
				strcpy(con_info->new_content_type, contentType.c_str());
				MHD_set_connection_value(connection, MHD_HEADER_KIND, CONTENT_TYPE, con_info->new_content_type);
			}
			MHD_get_connection_values(connection, MHD_HEADER_KIND, print_out_key, NULL);

			if (strmethod == "GET") {
				con_info->requestMethod = Request::GET;
				MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, get_get_parameters, con_info);
			}
			else if (strmethod == "POST") {
				//con_info->postprocessor = MHD_create_post_processor(connection, parameters.postBufferSize, iterate_post, (void*)con_info);
				
				//HACK - we don't want to use the postprocessor, we want raw json
				con_info->postprocessor = NULL;

				if (NULL == con_info->postprocessor)
				{
					//attempt to process the data ourselves
					con_info->customProcessor.active = true;
				}
				con_info->requestMethod = Request::POST;
			}

			*con_cls = (void*)con_info;
			return MHD_YES;
		}
		else {
			con_info = (connection_info*)*con_cls;
		}


		// second and next iterations
		string urlString(url);
		int ret = MHD_HTTP_SERVICE_UNAVAILABLE;

		Request request;

		if (con_info->requestMethod == Request::Method::POST) {
			if (con_info->customProcessor.active) {
				//handle custom data (e.g. json)
				if (*upload_data_size != 0) {
					con_info->customProcessor.data << string(upload_data, *upload_data_size);
					*upload_data_size = 0;
					return MHD_YES;
				}
				else {
					request.data = con_info->customProcessor.data.str();
				}
			}
			else {
				//handle post data with normal processor
				if (*upload_data_size != 0) {
					ret = MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
					*upload_data_size = 0;
					return MHD_YES;
				}
				else {
					map<string, FILE*>::iterator it;
					for (it = con_info->file_fields.begin(); it != con_info->file_fields.end(); it++) {
						if (it->second != NULL) {
							fflush(it->second);
							fclose(it->second);
							request.uploadedFiles[con_info->file_to_key_index[it->first]] = con_info->file_to_path_index[it->first];
						}
					}
				}
			}
		}



		//construct the request
		{
			request.url = urlString;

			//get referer
			{
				const char * referer = MHD_lookup_connection_value(connection
					, MHD_HEADER_KIND
					, MHD_HTTP_HEADER_REFERER);

				if (referer) {
					request.referer = referer;
				}
			}


			//get fields
			{
				request.fields = con_info->fields; //presume Poco::XMLString is same as string
			}

			//method
			{
				if (strmethod == "GET") {
					request.method = Request::GET;
				}
				else if (strmethod == "POST") {
					request.method = Request::POST;
				}
				else {
					request.method = Request::Unknown;
				}
			}

			//path
			{
				auto pathVector = ofSplitString(urlString, "/");
				request.path.assign(pathVector.begin(), pathVector.end());

				//strip element before first /
				request.path.pop_front();
			}

			//json
			{
				if (!request.data.empty()) {
					try {
						request.dataJson = json::parse(request.data);
					}
					catch (...) {

					}
				}
			}
		}

		//empty response is default until something fills it
		shared_ptr<Response> response;

		try {
			//handle requests
			{
				for (const auto & requestHandler : instance.requestHandlers) {
					if (requestHandler) {
						requestHandler->handleRequest(request, response);
					}
				}
			}

			//try to serve a page if we haven't got a handled response
			if (!response) {
				//we haven't made a response yet, let's try and serve a file instead

				//server-side redirect for /
				if (urlString == "/") {
					urlString = "/index.html";
				}

				response = serveFile(urlString, true);
			}
		}
		catch (std::exception & e) {
			response = make_shared<ResponseJson>(json{
				{"success", false},
				{"error", e.what()}
			});
		}

		//send the response
		if (response) { //this should always be true
			auto textResponse = dynamic_pointer_cast<ResponseText>(response);
			auto binaryResponse = dynamic_pointer_cast<ResponseBinary>(response);
			auto jsonResponse = dynamic_pointer_cast<ResponseJson>(response);
			auto redirectResponse = dynamic_pointer_cast<ResponseRedirect>(response);
			auto errorResponse = dynamic_pointer_cast<ResponseError>(response);

			if (textResponse) {
				ret = send_page(connection
					, textResponse->data.size()
					, textResponse->data.c_str()
					, textResponse->errorCode
					, textResponse->contentType);
			}
			else if (binaryResponse) {
				ret = send_page(connection
					, binaryResponse->data.size()
					, binaryResponse->data.getBinaryBuffer()
					, binaryResponse->errorCode
					, binaryResponse->contentType);
			}
			else if (jsonResponse) {
				json jsonResponseWrapped = {
					{"success" , true},
					{"data", jsonResponse->data}
				};

				auto responseString = jsonResponseWrapped.dump();

				ret = send_page(connection
					, responseString.size()
					, responseString.c_str()
					, jsonResponse->errorCode
					, jsonResponse->contentType);
			}
			else if (redirectResponse) {
				ret = send_redirect(connection
					, redirectResponse->location.c_str()
					, redirectResponse->errorCode);
			}
			else {
				if (!errorResponse) {
					errorResponse = make_shared<ResponseError>(404);
				}
				ret = send_error(connection
					, errorResponse->errorCode);
			}
		}

		return ret;
	}

	//-----------
	int Server::print_out_key(void * cls
		, enum MHD_ValueKind kind
		, const char * key
		, const char * value) {
		ofLogVerbose("ofxWebWidgets::Server") << ofVAArgsToString("%s = %s\n", key, value);
		return MHD_YES;
	}


	//-----------
	int Server::get_get_parameters(void *cls
		, enum MHD_ValueKind kind
		, const char * key
		, const char * value) {
		connection_info *con_info = (connection_info*)cls;
		if (key != NULL && value != NULL)
			con_info->fields[key] = value;
		return MHD_YES;
	}

	//-----------
	int Server::iterate_post(void * coninfo_cls
		, enum MHD_ValueKind kind
		, const char * key
		, const char * filename
		, const char * content_type
		, const char * transfer_encoding
		, const char * data
		, uint64_t off
		, size_t size) {

		auto & instance = Server::X();
		connection_info *con_info = (connection_info*)coninfo_cls;

		ofLogVerbose("ofxWebWidgets::Server") << "processing connection " << con_info->conn_id << endl;
		if (size > 0) {
			ofLogVerbose("ofxWebWidgets::Server") << "processing post field of size: " << size << "," << endl;
			if (transfer_encoding)
				ofLogVerbose("ofxWebWidgets::Server") << " transfer encoding: " << transfer_encoding << endl;
			if (content_type)
				ofLogVerbose("ofxWebWidgets::Server") << ", content: " << content_type << endl;
			if (filename)
				ofLogVerbose("ofxWebWidgets::Server") << ", filename: " << filename << endl;

			ofLogVerbose("ofxWebWidgets::Server") << ", " << key << endl;//": " <<data << endl;

			if (!filename) {
				char * aux_data = new char[off + size + 1];
				memset(aux_data, 0, off + size + 1);
				if (off > 0)
					memcpy(aux_data, con_info->fields[key].c_str(), off);

				memcpy(aux_data + off * sizeof(char), data, size);
				con_info->fields[key] = aux_data;
			}
			else {
				ofLogVerbose("ofxWebWidgets::Server") << "Server:" << "received file" << endl;
				if (con_info->file_fields.find(filename) == con_info->file_fields.end()) {
					con_info->file_fields[filename] = NULL;
					string uploadFolder(ofFilePath::join(instance.parameters.uploadFolder, ofGetTimestampString()));
					ofDirectory(uploadFolder).create();
					string path = ofFilePath::join(uploadFolder, filename);
					con_info->file_fields[filename] = fopen(ofToDataPath(path).c_str(), "ab");
					if (con_info->file_fields[filename] == NULL) {
						con_info->file_fields.erase(filename);
						return MHD_NO;
					}
					con_info->file_to_key_index[filename] = key;
					con_info->file_to_path_index[filename] = path;

				}
				if (size > 0) {
					if (!fwrite(data, size, sizeof(char), con_info->file_fields[filename])) {
						ofLogVerbose("ofxWebWidgets::Server") << "Server:" << "error on writing" << endl;
						con_info->file_fields.erase(filename);
						return MHD_NO;
					}
				}

			}
		}
		return MHD_YES;
	}

	//-----------
	void Server::request_completed(void * cls
		, struct MHD_Connection * connection
		, void ** con_cls
		, enum MHD_RequestTerminationCode toe) {
		connection_info *con_info = (connection_info*)*con_cls;


		if (NULL == con_info) {
			ofLogWarning("ofxWebWidgets::Server") << "request completed NULL connection";
			return;
		}

		if (con_info->requestMethod == Request::Method::POST) {
			MHD_destroy_post_processor(con_info->postprocessor);
		}

		delete con_info;
		*con_cls = NULL;

		auto & instance = Server::X();
		instance.activeClientCount--;
	}

	//-----------
	int Server::send_page(struct MHD_Connection *connection
		, long length
		, const char * page
		, int status_code
		, string contentType) {
		int ret;
		struct MHD_Response *response;


		response = MHD_create_response_from_data(length, (void*)page, MHD_NO, MHD_YES);
		if (!response) return MHD_NO;

		if (contentType != "") {
			MHD_add_response_header(response, CONTENT_TYPE, contentType.c_str());
		}

		ret = MHD_queue_response(connection, status_code, response);
		MHD_destroy_response(response);

		return ret;
	}

	//-----------
	int Server::send_redirect(struct MHD_Connection *connection
		, const char* location
		, int status_code) {

		int ret;
		struct MHD_Response *response;

		char data[] = "";
		response = MHD_create_response_from_data(0, data, MHD_NO, MHD_YES);
		if (!response) return MHD_NO;

		MHD_add_response_header(response, "Location", location);

		ret = MHD_queue_response(connection, status_code, response);
		MHD_destroy_response(response);

		return ret;
	}

	//----------
	int Server::send_error(struct MHD_Connection * connection
		, unsigned short errorCode) {
		auto & instance = Server::X();

		//try and load error file
		{
			ofFile file(instance.parameters.errorsFolder + "/" + ofToString(errorCode) + ".html"
				, ofFile::ReadOnly
				, false);
			if (file.exists()) {
				ofBuffer buffer;
				file >> buffer;
				return send_page(connection
					, buffer.size()
					, buffer.getBinaryBuffer()
					, MHD_HTTP_SERVICE_UNAVAILABLE);
			}
		}

		//otherwise just string something together
		{
			string html = "<h1>Error " + ofToString(errorCode) + "</h1>";
			return send_page(connection
				, html.size()
				, html.c_str()
				, MHD_HTTP_SERVICE_UNAVAILABLE);
		}
	}

	//-----------
	void Server::onExit(ofEventArgs &) {
		this->stop();
	}
}