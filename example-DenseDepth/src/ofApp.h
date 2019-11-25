// Copyright (C) 2018 Runway AI Examples
//
// This file is part of Runway AI Examples.
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

// RUNWAY
// www.runwayapp.ai

// Dense Depth
// Receive HTTP messages from Runway

// example by Roy Macdonald


#pragma once

#include "ofMain.h"

#include "ofxRunway.h"
#include "ofxGui.h"

// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener
{
public:
	void setup();
	void update();
	void draw();
	
	void keyReleased(int key);
	

	ofxRunway runway;
	ofImage runwayResult;

	ofVideoGrabber grabber;
	
	ofShader shader;
	ofEasyCam cam;

	ofVboMesh	sphereVbo;
	
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);

	
	// GUI parameters
	ofxPanel gui;
	ofParameter<float> depthScale = {"Depth Scale", 300, 0, 1000};
	ofParameter<float> pointSize = {"Point Size", 4, 1, 30};
	ofParameter<bool> bDrawPoints = {"Draw Points", true};
	ofParameter<int> pointStride = {"Point Stride", 10 ,1, 20};
	
};
