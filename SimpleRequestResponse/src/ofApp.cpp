#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofxWebWidgets::Server::Parameters parameters{

	};

	auto & server = ofxWebWidgets::Server::X();
	server.start(this, parameters);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

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

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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

//--------------------------------------------------------------
void ofApp::handleRequest(ofxWebWidgets::Request request
	, shared_ptr<ofxWebWidgets::Response> & response) {
	try {
		if (request.url == "/json") {
			auto responseJson = make_shared<ofxWebWidgets::ResponseJson>();
			responseJson->data = json{
				{ "frameRate",  ofGetFrameRate() },
				{ "frameNumber",  ofGetFrameNum() },
				{ "upTime",  ofGetElapsedTimef() }
			};
			response = responseJson;
		}
		else if (request.url == "/black") {
			ofSetBackgroundColor(0);
		}
		else if (request.url == "/white") {
			ofSetBackgroundColor(255);
		}
		else if (request.url == "/exception") {
			throw(std::bad_function_call());
		}
		else {
			//don't make a response
			return;
		}

		//standard response
		if (!response) {
			auto responseJson = make_shared<ofxWebWidgets::ResponseJson>();
			responseJson->data = json{
				{ "success",  true },
			};
			response = responseJson;
		}
	}
	catch (exception & e) {
		auto responseJson = make_shared<ofxWebWidgets::ResponseJson>();
		responseJson->data = json{
			{ "success",  false },
			{ "error",  e.what() }
		};
		response = responseJson;
	}
}