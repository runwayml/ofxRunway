#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1280,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
		
	//load a file as the style reference
	loadExtraImage();
	
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();

	// setup camera
	grabber.setup(640,480);


}
//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	if(grabber.isFrameNew() && !runway.isBusy()){
		
		ofxRunwayData data;
		
		if(!currentModel.extraImage.empty()){
			if(!extraImage.isAllocated()) return;
			data.setImage(currentModel.extraImage ,extraImage, OFX_RUNWAY_JPG);
		}
		
		// set content image
		data.setImage(currentModel.sendImage, grabber, OFX_RUNWAY_JPG);
		
		// query Runway
		runway.send(data);
	}
	runway.get(currentModel.receivedImage, runwayResult);
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	// draw camera feed
	grabber.draw(0,0);
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(640,0);}
	
	// draw style image (if loaded)
	if(extraImage.isAllocated()){extraImage.draw(0,480);}
	
	// display status
	
	stringstream ss;
	
	ss << "Current Model: " << currentModel.name << endl;
	
	if(!currentModel.extraImage.empty())
		ss << "Press SPACE to select a target image" << endl;
	
	
	ofRectangle r = runway.drawStatus(660, 520, true);
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0, 20, 0));
	
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == ' '){
		loadExtraImage();
	}
}
//--------------------------------------------------------------
void ofApp::loadExtraImage(){
	if(!currentModel.extraImage.empty()){
			auto resp = ofSystemLoadDialog("Select a " + currentModel.extraImage + " image to process...");

		if (!resp.bSuccess) {
			ofLogVerbose("ofApp") << "Window was closed or the user hit cancel." ;
		} else {
			ofLogVerbose("ofApp::loadExtraImage") <<  resp.getPath();
			extraImage.load(resp.getPath());
		// resize image (adjust as needed)
			extraImage.resize(600,400);
		}
	}
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
//--------------------------------------------------------------
