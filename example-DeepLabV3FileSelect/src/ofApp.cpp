#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//	ofSetLogLevel(OF_LOG_VERBOSE);
	loadFile();//contentImage, "Select a content image to process:");
	
	ofSetWindowShape(1200,800);
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	
}
//--------------------------------------------------------------
void ofApp::update(){
	runway.get("segmentation", runwayResult);
}
//--------------------------------------------------------------
void ofApp::exit(){
	runway.stopAndJoin();
}
//--------------------------------------------------------------
void ofApp::draw(){
	// draw content image (if loaded)
	if(contentImage.isAllocated()){contentImage.draw(0,0);}
	// draw image received from Runway
	if(runwayResult.isAllocated()){
		runwayResult.draw(600,0);
	
		// the image sent to runway and the one returned differ on size so we need to scale the mouse coordinates acoordingly so we get the correct values from the segmentationmap
		int x = ofGetMouseX();
		int y = ofGetMouseY();
		if(x < contentImage.getWidth()){
			// mouse is over the original image
			x = ofMap(x, 0, contentImage.getWidth(), 0, runwayResult.getWidth());
			y = ofMap(y, 0, contentImage.getHeight(), 0, runwayResult.getHeight());
		}else{
			//mouse is over the segmentation map
			x -= contentImage.getWidth();
		}
		
		//get the label based on the segmentation map, the image returned by runway and the mouse position
		auto label = ofxRunwayData::findSegmentationLabel(segmentationMap, runwayResult, x , y );
		if(!label.empty()){
			ofDrawBitmapStringHighlight(label, ofGetMouseX()+ 2, ofGetMouseY()+2);
		}
		// if pressing mouse overlay segmentation map with image
		if(ofGetMousePressed()){
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_SCREEN);
			runwayResult.draw(0,0, contentImage.getWidth(), contentImage.getHeight());
			ofPopStyle();
		}
	}
	
	// draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);

	stringstream ss;
	ss << "Press [key] : action " <<  endl;
	ss << "Press [ c ] : to select a content image."<< endl;
	ss << "Press [   ] : to send to Runway."<< endl;
	ss << "Press [ s ] : save the image returned by runway ."<< endl;
	ss << "move the mouse over the image or segmentation map to explore its labels" << endl;
	
	
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
	
	
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'c'){
		loadFile();
	}else if(key == ' '){
		sendFrameToRunway();
	}else if(key == 's'){
		if(runwayResult.isAllocated()){
			runwayResult.save(ofToDataPath("result_"+ ofGetTimestampString() +".png"));
		}
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
	
	runway.send("image", contentImage, OFX_RUNWAY_JPG);
	
}


// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){

	//here we catch the segmentation info using the ofxRunwayData utility function for it.
	ofxRunwayData::getSegmentationMap(segmentationMap, info);
	cout << info.dump(2) <<endl;
//	for(auto& s: segmentationMap){
//		cout << s.first << "  " << s.second << endl;
//	}
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
