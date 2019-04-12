#pragma once

#include "Constants.h"

namespace ofxWebWidgets {
	typedef std::deque<std::string> Path;

	struct Request {
		enum Method {
			POST,
			GET,
			Unknown // e.g. PUT
		};

		Method method;
		string referer;
		string url;
		string data;
		json dataJson;
		Path path;
		map<string, string> fields;
		map<string, string> uploadedFiles;

		string getPathString() const;
	};

	struct JsonRequest : Request {
		json incoming;
	};
}