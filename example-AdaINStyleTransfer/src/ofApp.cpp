#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1200,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	//load a file as the style reference
	loadFile();
	
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
		if(styleImage.isAllocated() && !runway.isBusy()){
		
		
		ofxRunwayData data;
		// set style image
		data.setImage("style_image",styleImage, OFX_RUNWAY_JPG);
		// set content image
		data.setImage("content_image", grabber, OFX_RUNWAY_JPG);
		// set preserve colour
		data.setBoolean("preserve_color",true);
		// set alpha
		data.setFloat("alpha",1.0);
		// query Runway
		runway.send(data);
		}
	}
	runway.get("image", runwayResult);
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	// draw style image (if loaded)
	if(styleImage.isAllocated()){styleImage.draw(600,0);}
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(0,400);}
	// draw camera feed
	grabber.draw(0,0,600,400);
	// display status
	
	ofDrawBitmapString("Press SPACE to select a style image", 620 ,420);
	runway.drawStatus(620, 440, true);
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == ' '){
		loadFile();
	}
}
//--------------------------------------------------------------
void ofApp::loadFile(){
	auto resp = ofSystemLoadDialog("Select a style image to process...");
	if (!resp.bSuccess) {
		ofLogVerbose("ofApp") << "Window was closed or the user hit cancel." ;
	} else {
		ofLogVerbose("ofApp::loadFile") <<  resp.getPath();
		styleImage.load(resp.getPath());
		// resize image (adjust as needed)
		styleImage.resize(600,400);
	}
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
