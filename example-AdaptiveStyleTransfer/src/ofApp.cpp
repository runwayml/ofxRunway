#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1200,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	// setup camera
	grabber.setup(640,480);
	
}
//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	if(grabber.isFrameNew()){
		runway.send("contentImage", grabber, OFX_RUNWAY_JPG);
	}
	runway.get("stylizedImage", runwayResult);
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(600,0);}
	// draw camera feed
	grabber.draw(0,0,600,400);
	// display status
	ofDrawBitmapString("Press SPACE to select a style image\n"+runway.getStateAsString(), 20 ,420);
}


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
