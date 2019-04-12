#pragma once

#include "../Constants.h"
#include "../RequestHandler.h"

namespace ofxWebWidgets {
	namespace Widgets {
		class Base : public RequestHandler {
		public:
			Base(const string & name)
				: name(name) { }

			virtual string getTypeName() const = 0;
			string getName() const;

			void handleRequest(const Request & request, shared_ptr<Response> & response) override;

			virtual json get() = 0;
			virtual json set(const json &) {
				throw(std::logic_error("Function not yet implemented"));
			}
		protected:
			string name;
		};
	}
}