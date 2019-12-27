#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
		ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	gui.setup("Pose Net");
	gui.add(maxDetections);
	gui.add(scoreThreshold);
	gui.add(estimationType);
	
	
//	video.setup(600,400);

	auto r = ofSystemLoadDialog();
	if(r.bSuccess){
		video.load(r.getPath());
	}
	
	ofSetWindowShape(1200,800);
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	
	poseFeatures.setup(runway.getOutputTypes());
	
	runway.start();	
	
}
//--------------------------------------------------------------
void ofApp::update(){
	video.update();
	
	if(video.isFrameNew() && !runway.isBusy()){
		
		ofxRunwayData data;
		data.setImage("image", video, OFX_RUNWAY_PNG);
		data.setInt("maxPoseDetections", maxDetections.get());
		data.setInt("estimationType",estimationType.get());
		data.setFloat("scoreThreshold", scoreThreshold.get());
		
		runway.send(data);
		
	}
	runway.get(poses, video.getWidth(), video.getHeight(), OFX_RUNWAY_POSE_NET);
	
}
//--------------------------------------------------------------
void ofApp::exit(){
	runway.stopAndJoin();
}
//--------------------------------------------------------------
void ofApp::draw(){
	// draw video video
	video.draw(0,0);
	// draw the poses received from Runway
	for(auto&p: poses){
		p.draw(poseFeatures);
	}
	
	// draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);
	
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
