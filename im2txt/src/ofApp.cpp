#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    black background
    ofSetBackgroundColor(0);
//      initialize empty string
    caption = "";
//      Use the localhost and the port 57200 that we define in Runway
    receiver.setup(57200);
}

//--------------------------------------------------------------
void ofApp::update(){
    while(receiver.hasWaitingMessages()){
        // get the next OSC message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        //        grab the data
        string data = m.getArgAsString(0);
        //        parse it to JSON
        results.parse(data);
        // From the JSONObject we want the key with the results
        // And from the array of captions (3) we want the first one
        // So we assign that caption to our global caption variable
        caption = results["results"][0]["caption"].asString();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString(caption, 10, 10);
}