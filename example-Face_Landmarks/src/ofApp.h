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

// Face Landmarks Demo:
// Receive HTTP messages from Runway
// Running Face Landmarks model

#pragma once

#include "ofMain.h"
#include "ofxRunway.h"

class ofApp : public ofBaseApp, public ofxRunwayListener{

	public:
		void setup();
		void update();
		void draw();

    ofxRunway runway;

	ofVideoGrabber video;
	
	vector<ofxRunwayFaceLandmarks> faceLandmarks;
	
// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);

	
};
