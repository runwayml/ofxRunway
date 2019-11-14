#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1200,800);
	
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
	if(grabber.isFrameNew()){
		runway.send("image", grabber, OFX_RUNWAY_JPG);
	}
	runway.get("image", runwayResult);
}
//--------------------------------------------------------------
void ofApp::draw(){
  // draw camera feed
  grabber.draw(0,0);
  // draw image received from Runway
  if(runwayResult.isAllocated()){runwayResult.draw(grabber.getWidth(),0);}
	
	// draw image received from Runway
	if(runwayResult.isAllocated()){
		runwayResult.draw(grabber.getWidth(),0);
		
		// the image sent to runway and the one returned differ on size so we need to scale the mouse coordinates acoordingly so we get the correct values from the segmentationmap
		int x = ofGetMouseX();
		int y = ofGetMouseY();
		if(x < grabber.getWidth()){
			// mouse is over the original image
			x = ofMap(x, 0, grabber.getWidth(), 0, runwayResult.getWidth());
			y = ofMap(y, 0, grabber.getHeight(), 0, runwayResult.getHeight());
		}else{
			//mouse is over the segmentation map
			x -= grabber.getWidth();
		}
		// if pressing mouse overlay segmentation map with image
		if(ofGetMousePressed()){
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_SCREEN);
			runwayResult.draw(0,0, grabber.getWidth(), grabber.getHeight());
			ofPopStyle();
		}
		//get the label based on the segmentation map, the image returned by runway and the mouse position
		auto label = ofxRunwayData::findSegmentationLabel(segmentationMap, runwayResult, x , y );
		if(!label.empty()){
			ofDrawBitmapStringHighlight(label, ofGetMouseX()+ 2, ofGetMouseY()+2);
		}

	}
	
	// draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(660, 440, true);
	
	stringstream ss;
	ss << "Move the mouse over the image or segmentation map to explore its labels." << endl;
	ss << "Press mouse to overlay segmentation map and input image." <<endl;
	
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
	

}
// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
	
	//here we catch the segmentation info using the ofxRunwayData utility function for it.
	ofxRunwayData::getSegmentationMap(segmentationMap, info);
	//	cout << info.dump(2) <<endl;
	//	for(auto& s: segmentationMap){
	//		cout << s.first << "  " << s.second << endl;
	//	}
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
