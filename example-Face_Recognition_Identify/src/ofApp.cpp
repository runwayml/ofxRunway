#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup() {
	ofSetWindowShape(1280,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	auto dialogResult = ofSystemLoadDialog("Select an reference image");
	if(!dialogResult.bSuccess || (dialogResult.bSuccess && !referenceFace.load(dialogResult.getPath()))){
		ofLogError("ofApp::setup") << "you need to load a face image as a reference. Will exit now.";
		ofExit();
	}
	
	gui.setup("Face Identification");
	gui.add(matchTolerance);
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
		
		ofxRunwayData data;
		data.setImage("input_image", video, OFX_RUNWAY_JPG);
		data.setImage("label_image", referenceFace, OFX_RUNWAY_JPG);
		data.setFloat("match_tolerance", matchTolerance.get());
		runway.send(data);
		
		
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
	
	gui.draw();
	
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

