#pragma once

#include "Base.h"

namespace ofxWebWidgets {
	namespace Widgets {
		template<typename Type>
		class EditableValue : public Base {
		public:
			EditableValue(const string & caption
				, function<Type()> get
				, function<void(Type)> set)
				: Base(caption)
				, getFunction(get)
				, setFunction(set) { }

			string getTypeName() const override {
				return "EditableValue";
			}

			json get() override {
				stringstream ss;
				ss << this->getFunction();
				return json(ss.str());
			}

			json set(const json & data) override {
				string textValue = data;
				Type value;
				stringstream ss(textValue);
				ss >> value;
				this->setFunction(value);
				return json();
			}
		protected:
			function<Type()> getFunction;
			function<void(Type)> setFunction;
		};
	}
}