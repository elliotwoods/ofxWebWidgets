#include "Base.h"

namespace ofxWebWidgets {
	namespace Widgets {
		//----------
		string Base::getName() const {
			return this->name;
		}

		//----------
		void Base::handleRequest(const Request & request, shared_ptr<Response> & response) {
			auto pathString = request.getPathString();

			if (pathString == "/get") {
				response = make_shared<ResponseJson>(this->get());
			}
			else if (pathString == "/set") {
				response = make_shared<ResponseJson>(this->set(request.data));
			}
		}
	}
}