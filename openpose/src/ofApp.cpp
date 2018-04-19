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
    
    width  = ofGetWidth();
    height = ofGetHeight();
    
//      Use the localhost and the port 57200 that we define in Runway
    receiver.setup(57200);

//      This are the pair of body connections we want to form.
//      Try creating new ones!
    connections = {
        {"Nose", "Left_Eye"},
        {"Left_Eye", "Left_Ear"},
        {"Nose", "Right_Eye"},
        {"Right_Eye", "Right_Ear"},
        {"Nose", "Neck"},
        {"Neck", "Right_Shoulder"},
        {"Neck", "Left_Shoulder"},
        {"Right_Shoulder", "Right_Elbow"},
        {"Right_Elbow", "Right_Wrist"},
        {"Left_Shoulder", "Left_Elbow"},
        {"Left_Elbow", "Left_Wrist"},
        {"Neck", "Right_Hip"},
        {"Right_Hip", "Right_Knee"},
        {"Right_Knee", "Right_Ankle"},
        {"Neck", "Left_Hip"},
        {"Left_Hip", "Left_Knee"},
        {"Left_Knee", "Left_Ankle"}
    };
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
//        grab the humans
        humans = results["results"]["humans"];

    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Choose between drawing just an ellipse
    // over the body parts or drawing connections
    // between them. or both!
    drawParts();
    drawConnections();
}

// A function to draw humans body parts as circles
void ofApp::drawParts(){
    for(int h = 0; h < humans.size(); h++) {
        ofxJSONElement human = humans[h];
        // Now that we have one human, let's draw its body parts
        for (int b = 0; b < human.size(); b++) {
            ofxJSONElement body_part = human[b];
            // Body parts are relative to width and weight of the input
            float x = body_part[1].asFloat();
            float y = body_part[2].asFloat();
            ofDrawEllipse(x * width, y * height, 10, 10);
        }
    }
}

// A function to draw connection between body parts
void ofApp::drawConnections(){
    for(int h = 0; h < humans.size(); h++) {
        ofxJSONElement human = humans[h];
        // Now that we have a human, let's draw its body
        // connections start by looping through all body
        // connections and matching only the ones we need.
        for(int c = 0; c < connections.size(); c++){
            ofxJSONElement start;
            ofxJSONElement end;
            // Check if we have a pair in the current body parts
            for(int b = 0; b < human.size(); b++) {
                ofxJSONElement body_part = human[b];
                string name = body_part[0].asString();
                if (name == connections[c][0]){
                    start = body_part;
                } else if (name == connections[c][1]){
                    end = body_part;
                }
            }
            if (start.size() > 0 && end.size() > 0){
                float x1 = start[1].asFloat() * width;
                float y1 = start[2].asFloat() * height;
                float x2 = end[1].asFloat() * width;
                float y2 = end[2].asFloat() * height;
                ofDrawLine(x1, y1, x2, y2);
            }
        }
    }
}