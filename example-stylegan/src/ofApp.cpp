#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1280, 800);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);

    ready = true;

    // setup Runway client
    ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
    runway.setup("http://localhost:8000/convert", "output");
    runway.start();
}

//--------------------------------------------------------------
void ofApp::update(){

    if (toSend) {

        // latent vector Z
        vector<float> latents;
        for (int i=0; i<512; i++) {
          latents.push_back(ofRandom(-3.5, 3.5));
        }

        // send bundle
        ofxRunwayBundle bundle;
        bundle.vectors["z"] = latents;
        bundle.floats["truncation"] = float(ofGetMouseX()) / float(ofGetWidth());
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
    if (outputTex.isAllocated()){
        outputTex.draw(0, 0);
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
