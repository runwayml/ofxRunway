// Copyright (C) 2018 Cristobal Valenzuela
//
// This file is part of RunwayML.
//
// RunwayML is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RunwayML is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RunwayML.  If not, see <http://www.gnu.org/licenses/>.
//
// ===========================================================================

// RUNWAY
// www.runwayml.com

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    black background
    ofSetBackgroundColor(0);
//      initialize empty string
    caption = "";
//      Use the localhost and the port 57200 that we define in Runway
    //receiver.setup(57200);
    ofxOscSenderSettings sendSettings;
    sendSettings.host ="127.0.0.1";
    sendSettings.port =57100;
    sendSettings.broadcast = true;
    sender.setup(sendSettings);
    ofxOscReceiverSettings recvSettings;
    recvSettings.port =57200;
    receiver.setup(recvSettings);
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