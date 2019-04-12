#pragma once

#include "Constants.h"

namespace ofxWebWidgets {
	enum ResponseType {
		Text,
		Binary,
		Json,
		Redirect,
		Error
	};

	struct Response {
		Response(const ResponseType responseType)
			: responseType(responseType) { }
		virtual ~Response() { }

		unsigned short errorCode = MHD_HTTP_OK;
		const ResponseType responseType;
	};	

	struct ResponseText : Response {
		ResponseText(const string & data)
			: Response(Text)
			, data(data) {}

		string data;
		string contentType = "text/html";
	};

	struct ResponseBinary : Response {
		ResponseBinary(const ofBuffer & data)
			: Response(Binary)
			, data(data) {}

		ofBuffer data;
		string contentType = "application/octet-stream";
	};

	struct ResponseJson : Response {
		ResponseJson(const json & data)
			: Response(Json)
			, data(data) {}

		json data;
		string contentType = "application/json";
	};

	struct ResponseRedirect : Response {
		ResponseRedirect(const string & location)
			: Response(Redirect)
			, location(location) {
			this->errorCode = MHD_HTTP_TEMPORARY_REDIRECT;
		}

		string location;
	};

	struct ResponseError : Response {
		ResponseError(unsigned short errorCode)
			: Response(Error) {
			this->errorCode = errorCode;
		}
	};

	shared_ptr<Response> serveFile(const std::string & path, bool addDocumentRoot);
}