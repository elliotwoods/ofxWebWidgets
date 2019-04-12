#include "Request.h"

namespace ofxWebWidgets {
	//----------
	string Request::getPathString() const {
		string pathString = "";
		for (const auto & level : this->path) {
			pathString += "/" + level;
		}
		return pathString;
	}
}