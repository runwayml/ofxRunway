#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1280, 800);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);

    width = 640;
    height = 480;
    ready = true;
    toSend = false;

    // setup Runway client
    ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
    runway.setup("http://localhost:8000/upscale", "output");
    runway.start();

    cam.setup(width, height);
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()) {
      input.setFromPixels(cam.getPixels());
      input.resize(int(width/4), int(height/4));
      if (ready) toSend = true;
    }

    if (toSend) {
        ofxRunwayBundle bundle;
        bundle.images["image"] = input.getPixels();
        runway.send(bundle);
        toSend = false;
        ready = false;
    }

    ofPixels processedPixels;
    while (runway.tryReceive(processedPixels)) {
        outputTex.loadData(processedPixels);
        ready = true;
    }

    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.draw(0, 0);
    input.draw(0, height, width, height);
    input.draw(width, height);
    if (outputTex.isAllocated()){
        outputTex.draw(width, 0);
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
