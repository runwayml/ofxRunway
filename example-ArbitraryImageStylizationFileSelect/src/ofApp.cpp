#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	// match sketch size to default model camera setup
	//  size(1800,400);
	//	ofSetLogLevel(OF_LOG_VERBOSE);
	loadFile(contentImage, "Select a content image to process:");
	loadFile(styleImage, "Select a style image to process...");
	
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
	//  background(0);
	// draw content image (if loaded)
	if(contentImage.isAllocated()){contentImage.draw(0,0);}
	// draw style image (if loaded)
	if(styleImage.isAllocated()){styleImage.draw(600,0);}
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(0,400);}
	
	// display status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);
	
	ofDrawBitmapString("Press 's' to select a style image\nPress 'c' to select a content image\nPress ' ' to send to Runway", r.getBottomLeft() + glm::vec3(0,20,0));
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 's'){
		loadFile(styleImage, "Select a style image to process...");
	}else if(key == 'c'){
		loadFile(contentImage, "Select a content image to process:");
	}else if(key == ' '){
		sendFrameToRunway();
	}
}
//--------------------------------------------------------------
void ofApp::loadFile(ofImage& img, const string& prompt){
	auto resp = ofSystemLoadDialog(prompt);
	if (!resp.bSuccess) {
		ofLogVerbose("ofApp") << "Window was closed or the user hit cancel." ;
	} else {
		ofLogVerbose("ofApp::loadFile") <<  resp.getPath();
		img.load(resp.getPath());
		// resize image (adjust as needed)
		img.resize(600,400);
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::sendFrameToRunway(){
	// skip if style image isn't loaded yet
	if(!styleImage.isAllocated()){
		return;
	}
	if(runway.isBusy()) return;
	
	ofxRunwayData data;
	
	// set style image
	data.setImage("style_image",styleImage, OFX_RUNWAY_JPG);
	// set content image
	data.setImage("content_image", contentImage, OFX_RUNWAY_JPG);
	// set interpolation
	data.setFloat("interpolation_weight",1.0);
	// query Runway
	runway.send(data);
	bWaitingForResponse = true;
}


// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
	ofLogNotice("ofApp::runwayInfoEvent") << info.dump();
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
