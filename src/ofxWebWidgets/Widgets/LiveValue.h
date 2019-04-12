#pragma once

#include "Base.h"

namespace ofxWebWidgets {
	namespace Widgets {
		template<typename Type>
		class LiveValue : public Base {
		public:
			LiveValue(const string & caption
				, function<Type()> get)
			: Base(caption)
			, getFunction(get) { }

			string getTypeName() const override {
				return "LiveValue";
			}

			json get() override {
				stringstream ss;
				ss << this->getFunction();
				return json(ss.str());
			}
		protected:
			function<Type()> getFunction;
		};}
}