#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	auto & server = ofxWebWidgets::Server::X();
	server.start();
	server.addRequestHandler(&this->widgetsSet);

	{
		auto widget = make_shared<ofxWebWidgets::Widgets::LiveValue<float>>(
			"Framerate"
			, []() {
				return ofGetFrameRate();
			}
		);
		this->widgetsSet.add(widget);
	}

	{
		auto widget = make_shared<ofxWebWidgets::Widgets::EditableValue<ofColor>>(
			"Color"
			, [this]() {
				return this->color;
			}
			, [this](ofColor value) {
				this->color = value;
			}
		);
		this->widgetsSet.add(widget);
	}

	{
		auto widget = make_shared<ofxWebWidgets::Widgets::Trackpad>(
			"Position"
			, ofVec2f(0, 0)
			, ofVec2f(ofGetWidth(), ofGetHeight())
			, [this]() {
			return this->position;
		}
			, [this](ofVec2f value) {
			this->position = value;
		}
		);
		this->widgetsSet.add(widget);
	}

	this->color = 0;
	this->position = {
		ofGetWidth() / 2.0f,
		ofGetHeight() / 2.0f
	};
}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(this->color);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofLine(this->position.x, 0, this->position.x, ofGetHeight());
	ofLine(0, this->position.y, ofGetWidth(), this->position.y);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	this->position.x = x;
	this->position.y = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	this->position.x = x;
	this->position.y = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
