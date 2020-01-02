#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	// match sketch size to default model camera setup
	//  size(1800,400);
	//	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofSetWindowShape(1200,800);
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	loadFile();

}
//--------------------------------------------------------------
void ofApp::update(){
	
	if(bWaitingForResponse){
		bWaitingForResponse = !runway.get(captions, contentImage.getWidth(), contentImage.getHeight(),OFX_RUNWAY_COCO);
	}
}
//--------------------------------------------------------------
void ofApp::draw(){
	//  background(0);
	// draw content image (if loaded)
	if(contentImage.isAllocated()){contentImage.draw(0,0);}

	for(auto&c: captions){// auto range for loops are awesome! :)
		c.draw();
	}
	
	
	// display status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);
	
	ofDrawBitmapString("Press 'c' to select a content image.\nPress SPACE for sending to runway\n RUNWAY IS " + (string)(runway.isBusy()?"BUSY":"NOT BUSY"), r.getBottomLeft() + glm::vec3(0,20,0));
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
//		sendFrameToRunway();
	}
}
//--------------------------------------------------------------
void ofApp::sendFrameToRunway(){
	if(runway.isBusy()) return;
	runway.send("image",contentImage, OFX_RUNWAY_JPG);
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
