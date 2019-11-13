#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//	ofSetLogLevel(OF_LOG_VERBOSE);
	loadFile();//contentImage, "Select a content image to process:");
	
	ofSetWindowShape(1200,800);
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	
}
//--------------------------------------------------------------
void ofApp::update(){
	if(bWaitingForResponse){
		runway.get("image", runwayResult);
	}
}
//--------------------------------------------------------------
void ofApp::draw(){
	// draw content image (if loaded)
	if(contentImage.isAllocated()){contentImage.draw(0,0);}
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(600,0);}
	
	// draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);
	
	ofDrawBitmapString("Press 'c' to select a content image\nPress ' ' to send to Runway", r.getBottomLeft() + glm::vec3(0,20,0));
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'c'){
		loadFile();
	}else if(key == ' '){
		sendFrameToRunway();
	}
}
//--------------------------------------------------------------
void ofApp::loadFile(){
	auto resp = ofSystemLoadDialog("Select a content image to process:");
	if (!resp.bSuccess) {
		ofLogVerbose("ofApp") << "Window was closed or the user hit cancel." ;
	} else {
		ofLogVerbose("ofApp::loadFile") <<  resp.getPath();
		contentImage.load(resp.getPath());
		// resize image (adjust as needed)
		contentImage.resize(600,400);
	}
}
//--------------------------------------------------------------
void ofApp::sendFrameToRunway(){
	// skip if content image isn't loaded yet
	if(!contentImage.isAllocated()){
		return;
	}
	if(runway.isBusy()) return;
	
	runway.send("image", contentImage, OFX_RUNWAY_JPG);
	
	bWaitingForResponse = true;
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
