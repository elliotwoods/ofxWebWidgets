#pragma once

#include "Constants.h"

namespace ofxWebWidgets {
	enum ResponseType {
		Text,
		Binary,
		Json,
		Redirect,
		Error404
	};

	struct Response {
		virtual ResponseType getResponseType() = 0;
		unsigned short errorCode = MHD_HTTP_OK;
	};	

	struct ResponseText : Response {
		ResponseType getResponseType() override {
			return ResponseType::Text;
		}
		string text;
		string contentType = "text/html";
	};

	struct ResponseBinary : Response {
		ResponseType getResponseType() override {
			return ResponseType::Binary;
		}
		ofBuffer data;
		string contentType = "application/octet-stream";
	};

	struct ResponseJson : Response {
		ResponseType getResponseType() override {
			return ResponseType::Json;
		}
		json data;
		string contentType = "application/json";
	};

	struct ResponseRedirect : Response {
		ResponseRedirect() {
			this->errorCode = MHD_HTTP_TEMPORARY_REDIRECT;
		}
		ResponseType getResponseType() override {
			return ResponseType::Redirect;
		}
		string location;
	};

	struct Response404 : Response {
		Response404() {
			this->errorCode = 404;
		}
		ResponseType getResponseType() override {
			return ResponseType::Error404;
		}
	};
}