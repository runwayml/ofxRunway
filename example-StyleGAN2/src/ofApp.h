#pragma once

#include "ofMain.h"
#include "ofxRunway.h"

// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener{
	
public:
	void setup();
	void update();
	void draw();
	void keyReleased(int key);
		
	ofxRunway runway;
	
	ofImage currentImg;
		
	vector<float> generate_random_z();
	void generate_image(vector<float> z, float truncation);
	
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);
	
	bool bWaitingForResponse = false;

	// GAN options
	float truncation = 1.5;
	float min_max_vals = 2;

};
