#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//	ofSetLogLevel(OF_LOG_VERBOSE);
	loadFile();//contentImage, "Select a content image to process:");
	
	gui.setup("Dense Cap");
	gui.add(maxDetections);
	guiListener = maxDetections.newListener([&](int&){
		// this is a lambda function. It is something relatively new to C++
		// but these are great and super useful. In this case this avoids having to create a new function as the callback for this listener.
		sendFrameToRunway();
	});
	
	
	ofSetWindowShape(1200,800);
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	
}
//--------------------------------------------------------------
void ofApp::update(){
	if(contentImage.isAllocated()){
		runway.get(captions,contentImage.getWidth(), contentImage.getHeight(),OFX_RUNWAY_DENSE_CAP);
	}
}
//--------------------------------------------------------------
void ofApp::exit(){
	runway.stopAndJoin();
}
//--------------------------------------------------------------
void ofApp::draw(){
	// draw content image (if loaded)
	if(contentImage.isAllocated()){contentImage.draw(0,0);}
	// draw the captions received from Runway
	for(auto&c: captions){
		c.draw();
	}
	
	// draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);
	
	stringstream ss;
	ss << "Press [ key ] : action " <<  endl;
	ss << "		 [  c  ] : to select a content image."<< endl;
	ss << "		 [space] : to send to Runway."<< endl;
	ss << "		 [  g  ] : enable/disable gui."<< endl;
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
	
	if(bDrawGui) gui.draw();
	
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'c'){
		loadFile();
	}else if(key == ' '){
		sendFrameToRunway();
	}else if(key == 'g'){
		bDrawGui ^= true;
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
//--------------------------------------------------------------
void ofApp::sendFrameToRunway(){
	// skip if content image isn't loaded yet
	if(!contentImage.isAllocated()){
		return;
	}
	if(runway.isBusy()) return;
	ofxRunwayData data;
	data.setImage("image", contentImage, OFX_RUNWAY_JPG);
	data.setInt("max_detections", maxDetections.get());
	runway.send(data);
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
