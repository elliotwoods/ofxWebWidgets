#pragma once

#include "Request.h"
#include "Response.h"

namespace ofxWebWidgets {
	class RequestHandler {
	public:
		virtual void handleRequest(const Request & request
			, shared_ptr<Response> & response) = 0;
	};
}
