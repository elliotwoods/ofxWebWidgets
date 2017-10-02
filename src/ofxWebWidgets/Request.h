#pragma once

#include "Constants.h"

namespace ofxWebWidgets {
	struct Request {
		enum Method {
			POST,
			GET,
			Unknown // e.g. PUT
		};

		Method method;
		string referer;
		string url;
		Path path;
		map<string, string> fields;
		map<string, string> uploadedFiles;
	};

	struct JsonRequest : Request {
		json incoming;
	};
}