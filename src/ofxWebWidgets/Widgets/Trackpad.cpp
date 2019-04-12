#include "Trackpad.h"

namespace ofxWebWidgets {
	namespace Widgets {
		//----------
		string Trackpad::getTypeName() const {
			return "Trackpad";
		}

		//----------
		json Trackpad::get() {
			const auto value = this->getFunction();
			return json({
				{
					"value"
					, {
						{ "x", value.x },
						{ "y", value.y }
					}
				},
				{
					"minimum"
					, {
						{ "x", this->minimum.x },
						{ "y", this->minimum.y }
					}
				},
				{
					"maximum"
					, {
						{ "x", this->maximum.x },
						{ "y", this->maximum.y }
					}
				}
			});
		}

		//----------
		json Trackpad::set(const json & data) {
			ofVec2f value;
			value.x = data["x"];
			value.y = data["y"];
			this->setFunction(value);
			return json();
		}
	}
}