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
	
	grabber.setup(640,480);

}
//--------------------------------------------------------------
void ofApp::update(){

	grabber.update();
	if(grabber.isFrameNew() && bSendImage){
		runway.send("image", grabber, OFX_RUNWAY_JPG);
	}
	runway.get(captions, grabber.getWidth(), grabber.getHeight(),OFX_RUNWAY_COCO);
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	grabber.draw(0, 0);
	for(auto&c: captions){// auto range for loops are awesome! :)
		c.draw();
	}
	
	// display status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(grabber.getWidth() + 20, 20, true);
	
	stringstream ss;
	ss << "Press SPACE to toggle sending images to Runway." << endl;
	ss << "Sending Image: " << boolalpha << bSendImage << endl;
	ss << "RUNWAY IS " << (string)(runway.isBusy()?"BUSY":"NOT BUSY");
	
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
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
void ofApp::keyReleased(int key){
	if(key == ' ')bSendImage ^= true;
}
