#pragma once

#include "Base.h"
#include "../Constants.h"
#include "../RequestHandler.h"

namespace ofxWebWidgets {
	namespace Widgets {
		class Set : public map<size_t, shared_ptr<Base>>, public RequestHandler {
		public:
			void add(const shared_ptr<Base> &);
			void handleRequest(const Request & request, shared_ptr<Response> & response) override;
		protected:
		};
	}
}