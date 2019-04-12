#include "Set.h"

namespace ofxWebWidgets {
	namespace Widgets {
		//----------
		void Set::add(const shared_ptr<Base> & widget) {
			size_t nextID = 0;
			if (!this->empty()) {
				nextID = this->rbegin()->first + 1;
			}
			this->insert({ nextID, widget });
		}

		//----------
		void Set::handleRequest(const Request & request, shared_ptr<Response> & response) {
			//build the inner request
			auto pathString = request.getPathString();
			if(pathString.empty() || pathString == "/") {
				auto redirectURL = request.url;
				if (*redirectURL.rbegin() == '/') {
					redirectURL.pop_back();
				}
				redirectURL += "/widgetsSet.html";
				response = make_shared<ResponseRedirect>(redirectURL);
			}
			else if (pathString == "/widgetsSet.html") {
				response = serveFile("widgets/widgetsSet.html", true);
			}
			else if (pathString == "/widgetsList.json") {
				//return a list of indexes
				json data;
				for (const auto & it : *this) {
					auto widgetData = json{
						{ "index", it.first },
						{"name", it.second->getName()},
						{"typeName", it.second->getTypeName()}
					};
					data.push_back(widgetData);
				}
				response = make_shared<ResponseJson>(data);
			}
			else if (request.path.at(0) == "widget") {
				if (request.path.size() >= 2) {
					auto widgetIndex = ofToInt(request.path[1]);
					auto findWidget = this->find(widgetIndex);
					if (findWidget != this->end()) {
						auto widgetRequest = request;

						widgetRequest.path.pop_front();
						widgetRequest.path.pop_front();

						findWidget->second->handleRequest(widgetRequest, response);
					}
				}
			}
		}
	}
}