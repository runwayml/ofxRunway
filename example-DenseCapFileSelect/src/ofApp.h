// Copyright (C) 2019 RunwayML Examples
// 
// This file is part of RunwayML Examples.
// 
// Runway-Examples is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Runway-Examples is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with RunwayML.  If not, see <http://www.gnu.org/licenses/>.
// 
// ===========================================================================

// RUNWAYML
// www.runwayml.com

// COCO-SSD Demo:
// Receive HTTP messages from Runway
// Running COCO-SSD model
// example by Roy Macdonald, ported from Processing example by George Profenza

#pragma once

#include "ofMain.h"
#include "ofxRunway.h"

#include "ofxGui.h"
// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener{
	
public:
	void setup();
	void update();
	void draw();
	
	void exit();
	
	void keyReleased(int key);
	
	void loadFile();
	
	ofxRunway runway;
	
	ofImage contentImage;
	
	void sendFrameToRunway();
	
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);
	
	
	// DenseCap returns captions only
	vector<ofxRunwayCaption> captions;
	
	// gui stuff
	
	ofParameter<int>maxDetections = {"Max Detections", 10, 1, 50};

	ofxPanel gui;

	bool bDrawGui = true;
	
	// we need this event listener so we can make automatic new queries to runway when the gui changes
	ofEventListener guiListener;
	
};
