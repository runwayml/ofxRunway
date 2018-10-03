#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);

    host = "http://localhost:23100";
    
    width = 1024;
    height = 512;
    
    // setup Runway client
    ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
    runway.setup(host);
    runway.start();
    
    input.load("test.jpg");
    input.resize(width, height);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (toSend) {
        runway.send(input.getPixels());
        toSend = false;
    }
    
    ofPixels processedPixels;
    while (runway.tryReceive(processedPixels)) {
        outputTex.loadData(processedPixels);
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    input.draw(0, 0);
    if (outputTex.isAllocated()){
        outputTex.draw(0, height);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' '){
        toSend = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
