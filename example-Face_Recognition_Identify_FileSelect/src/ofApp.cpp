#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup() {
	ofSetWindowShape(1280,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	// setup Runway
	gui.setup("Face Identification");
	gui.add(match_tolerance);
	matchToleranceListener = match_tolerance.newListener([&](float&){
		sendFrameToRunway();
	});
	
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	
	loadContentImage();
	loadReferenceImage();
	
	

}
//--------------------------------------------------------------
void ofApp::update(){
	runway.get(faces, contentImage.getWidth(), contentImage.getHeight(), OFX_RUNWAY_FACE_DETECTION);
}

//--------------------------------------------------------------
void ofApp::exit(){
	runway.stopAndJoin();
}
//--------------------------------------------------------------
void drawImg(const ofImage &img, const ofRectangle& r){
	if(!img.isAllocated()) return;
	//this will scale the image proportionally so it fits in the passed rectangle.
	ofRectangle s (0,0, img.getWidth(), img.getHeight());
	s.scaleTo(r);
	s.y = 0;
	img.draw(s);
}
//--------------------------------------------------------------
void ofApp::draw() {
//  background(0);
  
	ofSetColor(255);
	
	drawImg(contentImage, {0,0,600,600});
	drawImg(referenceImage, {600,0,600,600});

	
	for(auto&f: faces){
		f.draw();
	}
	
	
	
	auto r = runway.drawStatus(20, 620, true);
	
	stringstream ss;
	ss << "Press [ key ] : action " <<  endl;
	ss << "		 [  c  ] : to select a content image."<< endl;
	ss << "		 [  r  ] : to select a reference image."<< endl;
	ss << "		 [  g  ] : enable/disable gui."<< endl;
	
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
	if(bDrawGui) gui.draw();
	
  
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
  if(key == 'r'){
	  loadReferenceImage();
  }else	if(key == 'c'){
	  loadContentImage();
  }else if(key == 'g'){
	  bDrawGui ^= true;
  }
}
//--------------------------------------------------------------
void ofApp::loadImg(ofImage& img, string imgname){
	auto resp = ofSystemLoadDialog("Select " + imgname);
	if(!resp.bSuccess || (resp.bSuccess && !img.load(resp.getPath()))) {
		ofLogError("ofApp::loadImg " ) << "failed loading " << imgname;
		return ;
	}
	sendFrameToRunway();
}
//--------------------------------------------------------------
void ofApp::loadContentImage(){
	loadImg(contentImage, "Content Image");
}
//--------------------------------------------------------------
void ofApp::loadReferenceImage(){
	loadImg(referenceImage, "Reference Image");
}
//--------------------------------------------------------------
void ofApp::sendFrameToRunway(){
	// skip if content image isn't loaded yet
	if(!contentImage.isAllocated() || !referenceImage.isAllocated()){
		return;
	}
	
	if(runway.isBusy()) return;
	
	
	ofxRunwayData data;
	
	data.setImage("input_image", contentImage, OFX_RUNWAY_JPG);
	data.setImage("label_image", referenceImage, OFX_RUNWAY_JPG);
	
	data.setFloat("match_tolerance", match_tolerance.get());
	runway.send(data);
	
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

