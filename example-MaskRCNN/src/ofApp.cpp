#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1280,800);
	
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
	if(grabber.isFrameNew() && !runway.isBusy()){
		
		ofxRunwayData data;
			
		// set content image
		data.setImage("image", grabber, OFX_RUNWAY_JPG);
		data.setString("category", "person");
		// query Runway
		runway.send(data);
	}
	runway.get("output", runwayResult);

}
//--------------------------------------------------------------
void ofApp::draw(){
	
	// draw camera feed
	grabber.draw(0,0);
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(640,0);}
	
	if(mask.isAllocated()){mask.draw(0, 480);}
	
	runway.drawStatus(660, 520, true);
	
	
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
