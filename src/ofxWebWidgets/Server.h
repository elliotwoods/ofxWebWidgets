#pragma once

#include "Constants.h"
#include "Request.h"
#include "RequestHandler.h"

namespace ofxWebWidgets {
	class Server {
	public:
		struct Parameters {
			string staticRoot = "http/DocumentRoot";
			string uploadFolder = "http/upload";
			string errorsFolder = "http/errors";
			unsigned short port = 8888;
			unsigned short maximumClients = 100;
			unsigned short maximumActiveClients = 4;
			size_t postBufferSize = 64 * 1024;
		};

		static Server & X();
		virtual ~Server();

		void start(const Parameters & = Parameters());
		void stop();
		bool isRunning() const;

		void addRequestHandler(RequestHandler *);
		void removeRequestHandler(RequestHandler *);
		set<RequestHandler *> & getRequestHandlers();
		const set<RequestHandler *> & getRequestHandlers() const;

		const Parameters & getParameters() const;
	private:
		Server();
		Parameters parameters;

		struct MHD_Daemon * daemon;
		set<RequestHandler *> requestHandlers;

		atomic<size_t> activeClientCount{ 0 };

		static enum MHD_Result handleConnection(void * cls
			, struct MHD_Connection * connection
			, const char * url
			, const char * method
			, const char * version
			, const char * upload_data
			, size_t * upload_data_size
			, void ** con_cls);

		static MHD_Result print_out_key(void *cls
			, enum MHD_ValueKind kind
			, const char * key
			, const char * value);

		static MHD_Result get_get_parameters(void *cls
			, enum MHD_ValueKind kind
			, const char * key
			, const char * value);

		static MHD_Result send_page(struct MHD_Connection *connection
			, long length
			, const char * page
			, int status_code
			, std::string contentType = "");

		static MHD_Result send_redirect(struct MHD_Connection *connection
			, const char * location
			, int status_code);

		static void request_completed(void *cls
			, struct MHD_Connection * connection
			, void ** con_cls
			, enum MHD_RequestTerminationCode toe);

		static int iterate_post(void *coninfo_cls
			, enum MHD_ValueKind kind
			, const char * key
			, const char * filename
			, const char * content_type
			, const char * transfer_encoding
			, const char * data
			, uint64_t off
			, size_t size);

		static MHD_Result send_error(struct MHD_Connection * connection
			, unsigned short errorCode);

		void onExit(ofEventArgs &);
	};
}

