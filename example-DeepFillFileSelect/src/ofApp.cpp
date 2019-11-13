#include "ofApp.h"
#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	ofSetWindowShape(1200,800);

	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	loadFile();
	
}
//--------------------------------------------------------------
void ofApp::update(){
	runway.get("inpainted", runwayResult);
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
	if(contentImage.isAllocated())	contentImage.draw(0,0);
	
	if(mask.isAllocated()){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		mask.draw(0,0);
		ofPopStyle();
	}
	// draw mask image (if loaded)
	if(mask.isAllocated()){mask.draw(600,0);}
	// draw image received from Runway
	if(runwayResult.isAllocated()){runwayResult.draw(0,400);}
	
	// display status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
	ofRectangle r = runway.drawStatus(620, 440, true);
	
	stringstream ss;
	ss << "Press [key] : action " <<  endl;
	ss << "Press [ c ] : to select a content image."<< endl;
	ss << "Press [   ] : to send to Runway."<< endl;
	ss << "Press [ q ] : reduce brush size ."<< endl;
	ss << "Press [ w ] : increase brush size ."<< endl;
	ss << "Press [ m ] : clear mask ."<< endl;
	ss << endl;
	ss << "Current brush size: " << brushSize << endl;
	ss << endl << "Drag mouse over image to mask it. Press SPACE when ready.";
	
	
	ofDrawBitmapString(ss.str(), r.getBottomLeft() + glm::vec3(0,20,0));
}
//--------------------------------------------------------------
void ofApp::clearMask(){
	if(mask.getWidth() != contentImage.getWidth() || mask.getHeight() != contentImage.getHeight()){
		mask.allocate(contentImage.getWidth(), contentImage.getHeight(), GL_RGB);
	}
	mask.begin();
	ofClear(0);
	mask.end();
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'c'){
		loadFile();
	}else if(key == 'q'){
		brushSize -= 3;
		if(brushSize < 1) brushSize = 1;
	}else if(key == 'w'){
		brushSize += 3;
	}else if(key == 'm'){
		clearMask();
	}else if(key == ' '){
		sendFrameToRunway();
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
		clearMask();
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::sendFrameToRunway(){
	// skip if style image isn't loaded yet
	if(!contentImage.isAllocated()){
		return;
	}
	if(runway.isBusy()) return;
	
	ofxRunwayData data;
	
	ofPixels pix;
	
	mask.readToPixels(pix);
	// set mask image
	data.setImage("mask", pix, OFX_RUNWAY_JPG);
	// set content image
	data.setImage("image", contentImage, OFX_RUNWAY_JPG);
	// query Runway
	runway.send(data);
}
//--------------------------------------------------------------
void ofApp::drawMouseToMask(int x, int y){
	mask.begin();
	ofSetColor(255);
	ofDrawCircle(x, y, brushSize);
	mask.end();
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	drawMouseToMask(x,y);
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	drawMouseToMask(x,y);
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	drawMouseToMask(x,y);
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
