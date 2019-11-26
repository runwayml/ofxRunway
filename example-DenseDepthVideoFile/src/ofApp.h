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
// This example does not use runway direcly but its renders instead.
// Runway can receive a video or movie file and process each of its frames.
// We will use the generated depth maps from the Dense Depth model.
// You can use a video file straight away but for better results
// transform a video file into an image sequence.
// You can do this with ffmpeg, a command line library for dealing with video, use the following command.
// ffmpeg -i path-to-your-movie-file.mp4 -f image2 movieFrames-%05d.png
// In any case make sure to output an image sequence. Run the model, press export and wait.
// Once done, run this app. In the first file dialog choose the video file (not the video frames).
// On the next file dialog choose the folder where the output of runway is.


// example by Roy Macdonald


#pragma once

#include "ofMain.h"
#include "ofxGui.h"

// If you dont ofxGrabCam, get it from https://github.com/elliotwoods/ofxGrabCam
// If you have, unncomment the line below. It is much nicer to navigate the 3D scene with it.
//#define USE_GRAB_CAM
 
#ifdef USE_GRAB_CAM
#include "ofxGrabCam.h"
#endif
class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void mousePressed(int x, int y, int button);
	
	void keyReleased(int key);

	ofVideoPlayer player;
	vector<ofPixels> depthImgs;
	
	ofShader shader;
#ifdef USE_GRAB_CAM
		ofxGrabCam cam;
#else
	ofEasyCam cam;
#endif
	ofVboMesh	sphereVbo;
	

	ofTexture depthTex;
	
	
	// GUI parameters
	ofxPanel gui;
	ofParameter<float> depthScale = {"Depth Scale", 300, 0, 10000};
	ofParameter<float> pointSize = {"Point Size", 4, 1, 30};
	ofParameter<bool> bDrawPoints = {"Draw Points", true};
	ofParameter<int> pointStride = {"Point Stride", 10 ,1, 20};

	void loadDepthImages(const string& dirpath);
	
	void loadDialog();
	void load(const string& video, const string& depthDir);
	
	bool bDrawGui = true;
	bool bDrawVideo = true;
	bool bDrawDepth = true;
	bool bDrawInfo = true;
	
	
	ofRectangle camViewport;
#ifndef USE_GRAB_CAM
	void findCursor(const glm::vec2& mouse);
	ofShortPixels sampleNeighbourhood;
#endif
};
