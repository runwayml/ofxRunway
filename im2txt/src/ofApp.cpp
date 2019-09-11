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
    // setup OSC sender
    ofxOscSenderSettings sendSettings;
    sendSettings.host ="127.0.0.1";
    sendSettings.port =57100;
    sendSettings.broadcast = true;
    sender.setup(sendSettings);
    // setup OSC receiver
    ofxOscReceiverSettings recvSettings;
    recvSettings.port =57200;
    receiver.setup(recvSettings);
    // send connect message to Runway
    connect();
}

//--------------------------------------------------------------
void ofApp::update(){
    while(receiver.hasWaitingMessages()){
        // get the next OSC message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        // if it's /data
        if(m.getAddress() == "/data"){
            //        grab the data
            string data = m.getArgAsString(0);
            //        parse it to JSON
            results.parse(data);
            //        retrieve the caption property {"caption":"im2txt result here"}
            caption = results["caption"].asString();
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString(caption, 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'c'){
        connect();
    }
    if(key == 'c'){
        disconnect();
    }
}

//--------------------------------------------------------------
void ofApp::connect(){
    ofxOscMessage m;
    m.setAddress("/server/connect");
    sender.sendMessage(m, false);
}

//--------------------------------------------------------------
void ofApp::disconnect(){
    ofxOscMessage m;
    m.setAddress("/server/disconnect");
    sender.sendMessage(m, false);
}

