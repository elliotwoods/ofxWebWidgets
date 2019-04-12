#include "Response.h"
#include "Server.h"

using namespace std;

struct ContentType {
	string mimeType;
	bool binary;
};

static map<string, ContentType> contentTypes = {
	{ "html",{ "text/html", false } }
	,{ "htm",{ "text/html", false } }
	,{ "css",{ "text/css", false } }
	,{ "js",{ "application/javascript", false } }
	,{ "json",{ "application/json", false } }
	,{ "svg",{ "image/svg+xml", false } }
	,{ "jpg",{ "image/jpeg", true } }
	,{ "jpeg",{ "image/jpeg", true } }
	,{ "gif",{ "image/gif", true } }
	,{ "png",{ "image/png", true } }
	,{ "woff",{ "font/woff", true } }
	,{ "woff2",{ "font/woff2", true } }
	,{ "ttf",{ "font/ttf", true } }
};

namespace ofxWebWidgets {
	//----------
	shared_ptr<Response> serveFile(const string & path, bool addDocumentRoot) {
		auto & instance = Server::X();

		auto pathToServe = path;

		if (addDocumentRoot) {
			if (!pathToServe.empty() && * pathToServe.begin() != '/') {
				pathToServe = "/" + pathToServe;
			}
			pathToServe = instance.getParameters().staticRoot + pathToServe;
		}

		ofFile file(pathToServe
			, ofFile::ReadOnly
			, true);

		if (!file.exists() || file.isDirectory()) {
			return make_shared<ResponseError>(404);
		}
		else {
			ofBuffer buf;
			file >> buf;

			auto extension = ofToLower(ofFilePath::getFileExt(pathToServe));
			
			auto findContentType = contentTypes.find(extension);
			if (findContentType != contentTypes.end()) {
				//found matching content type
				const auto & contentType = findContentType->second;
				if (contentType.binary) {
					//known binary type
					auto response = make_shared<ResponseBinary>(buf);
					response->contentType = contentType.mimeType;
					return response;
				}
				else {
					//known text type
					auto response = make_shared<ResponseText>(buf.getText());
					response->contentType = contentType.mimeType;
					return response;
				}
			}
			else {
				//unknown type - send as octet-stream
				auto response = make_shared<ResponseBinary>(buf);
				return response;
			}
		}
	}
}