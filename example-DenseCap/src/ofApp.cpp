#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetWindowShape(1200,800);
	
	
	gui.setup("Dense Cap");
	gui.add(maxDetections);
	
	
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	grabber.setup(640,480);
}
//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	if(grabber.isFrameNew()){
		if(!runway.isBusy()){
			ofxRunwayData data;
			data.setImage("image", grabber, OFX_RUNWAY_JPG);
			data.setInt("max_detections", maxDetections.get());
			runway.send(data);
		}
	}

	runway.get(captions,grabber.getWidth(), grabber.getHeight(),OFX_RUNWAY_DENSE_CAP);
	
}
//--------------------------------------------------------------
void ofApp::exit(){
	runway.stopAndJoin();
}
//--------------------------------------------------------------
void ofApp::draw(){
	// draw content image (if loaded)
	grabber.draw(0,0);
	// draw the captions received from Runway
	for(auto&c: captions){
		c.draw();
	}
	
	// draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 40, true);
	
	stringstream ss;
	ss << "Press [ key ] : action " <<  endl;
	ss << "		 [  g  ] : enable/disable gui."<< endl;
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
	
	if(bDrawGui) gui.draw();
	
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'g'){
		bDrawGui ^= true;
	}
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
