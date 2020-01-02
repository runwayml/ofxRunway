#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup() {
  ofSetWindowShape(1280,800);
  
  //uncomment the following line if you want a verbose log (which means a lot of info will be printed)
  //ofSetLogLevel(OF_LOG_VERBOSE);
  
  // setup Runway
  runway.setup(this, "http://localhost:8000");
  runway.start();
  
  // setup the video grabber (camera)
  video.setup(640,480);
  
}
//--------------------------------------------------------------
void ofApp::update(){
	video.update();
	
	if(video.isFrameNew() && !runway.isBusy()){
		runway.send("image", video);
	}
	runway.get(faceRects, video.getWidth(), video.getHeight());
}
//--------------------------------------------------------------
void ofApp::draw() {
  
  video.draw(0,0);
// Draw the face rectangles
	ofPushStyle();
	ofNoFill();
	ofSetColor(ofColor::red);
	for(auto&r: faceRects){
		ofDrawRectangle(r);
	}
	ofPopStyle();
  // Draw status
  runway.drawStatus(20, 520, true);
  
}
//--------------------------------------------------------------
// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
	ofLogNotice("ofApp::runwayInfoEvent") << info.dump(2);
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
//--------------------------------------------------------------

