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

// OpenPose Demo:
// Receive OSC messages from Runway
// running the OpenPose model

// Crist—bal Valenzuela, George Profenza (adapting Cris' Processing example to OF)
// April 2018
// cv965@nyu.edu

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxJSON.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void drawParts();
        void drawConnections();

		ofxOscReceiver receiver;
    
        // This array will hold the results parsed from the OSC message string
        ofxJSONElement results;
        // This array will hold all the humans detected
		ofxJSONElement humans;
    
        vector<vector<string>> connections;;
        int width,height;
};
