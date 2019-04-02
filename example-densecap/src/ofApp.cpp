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
    runway.setup("http://localhost:9000/");
    runway.start();

    cam.setup(width, height);
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()) {
      input.setFromPixels(cam.getPixels());
      if (ready) toSend = true;
    }

    if (toSend) {
        ofxRunwayBundle bundle;
        bundle.address = "caption";
        bundle.images["image"] = input.getPixels();
        runway.send(bundle);
        toSend = false;
        ready = false;
    }

    ofxRunwayBundle bundleToReceive;
    while (runway.tryReceive(bundleToReceive)) {
        captions = bundleToReceive.vectorsS["captions"];
        scores = bundleToReceive.vectorsF["scores"];
        boxes = bundleToReceive.vectorsV["boxes"];
        ready = true;
    }


    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){    
    ofBackground(0);
    input.draw(0, 0);
    for (int i=0; i<min(int(captions.size()), 4); i++) {//captions.size(); i++) {
        vector<float> box = boxes[i];
        string caption = captions[i];
        float score = scores[i];

        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255);
        ofDrawRectangle(box[0], box[1], box[2]-box[0], box[3]-box[1]);
        ofDrawBitmapStringHighlight(caption, box[0]+5, box[1]+20);
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
