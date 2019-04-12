#pragma once

#include "Base.h"

namespace ofxWebWidgets {
	namespace Widgets {
		class Trackpad : public Base {
		public:
			Trackpad(const string & caption
				, ofVec2f minimum
				, ofVec2f maximum
				, function<ofVec2f()> get
				, function<void(ofVec2f)> set)
				: Base(caption)
				, minimum(minimum)
				, maximum(maximum)
				, getFunction(get)
				, setFunction(set) { }

			string getTypeName() const override;

			json get() override;
			json set(const json &) override;
			
		protected:
			ofVec2f minimum;
			ofVec2f maximum;
			function<ofVec2f()> getFunction;
			function<void(ofVec2f)> setFunction;
		};
	}
}