#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	grabber.setup(600,400);
	
	ofSetWindowShape(1200,800);
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	
	runway.start();
	
}
//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	
	if(grabber.isFrameNew()){
		
		if(!runway.isBusy()){
			runway.send("image", grabber, OFX_RUNWAY_JPG, 600, 400);
		}
	}
	
	ofxRunwayData dataToReceive;
	while (runway.tryReceive(dataToReceive)) {
		dataToReceive.getImage("image", runwayResult);
		runwayResult.update();
		dataToReceive.getPoses(poses, runwayResult.getWidth(), runwayResult.getHeight(),OFX_RUNWAY_PIF_PAF);
//		string keypoints;
		
//		dataToReceive.getString("keypoints", keypoints);
//		cout << "---------------------------------------------------" << endl;
//		cout << keypoints << endl;
	}
	
	
//	runway.get(poses,grabber.getWidth(), grabber.getHeight());
	
}
//--------------------------------------------------------------
void ofApp::exit(){
	runway.stopAndJoin();
}
//--------------------------------------------------------------
void ofApp::draw(){
	// draw video grabber
	grabber.draw(0,0);
	// draw the poses received from Runway
	if(runwayResult.isAllocated()) runwayResult.draw(grabber.getWidth() + 10, 0);
	for(auto&p: poses){
		p.draw();
	}
	
	
	runway.drawStatus(620, 440, true);
	
	
}


// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
	ofLogNotice("ofApp::runwayInfoEvent") << endl << info.dump(2) <<endl;
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
