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

// AdaIN-Style-Transfer
// Receive HTTP messages from Runway
// Running AdaIN-Style-Transfer model
// example by Roy Macdonald, ported from Processing example by George Profenza


#pragma once

#include "ofMain.h"
#include "ofxRunway.h"

// simple struct used to store info about the model
struct Model{
	Model(){}
	Model(string _name, string _sendImage, string _receivedImage): 
	name(_name), sendImage(_sendImage), receivedImage(_receivedImage){}
	Model(string _name, string _sendImage, string _extraImage, string _receivedImage): 
	name(_name), sendImage(_sendImage), receivedImage(_receivedImage), extraImage(_extraImage){}
	string name;
	string sendImage;
	string receivedImage;
	string extraImage;

};
// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener{
	
	
	/// SUPER IMPORTANT!!!
	/// Uncomment from the list below the model that you're going to use.
	/// Make sure there is only one uncommented
	Model currentModel =
		//Model name                     Sent Image            Received image
	// {"DeepPrivacy"                   ,  "image"            ,  "anonymized_image" };
//	 {"Image-Super-Resolution"        ,  "image"            ,  "upscaled"         };
//	 {"Human-Segmentation"            ,  "input_image"      ,  "output_image"     };
	// {"CycleGAN"                      ,  "image"            ,  "image"            };
	// {"SRFBN"                         ,  "input_image"      ,  "output_image"     };
	// {"style2paints"                  ,  "input_image"      ,  "output_image"     };
//	 {"Pix2Pix"                       ,  "image"            ,  "image"            };
	// {"ESRGAN"                        ,  "image"            ,  "upscaled"         };
	// {"edge-connect"                  ,  "input_image"      ,  "output_image"     };
	// {"LiteFlowNet"                   ,  "input_image"      ,  "output_image"     };
	// {"Adaptive-Style-Transfer"       ,  "contentImage"     ,  "stylizedImage"    };
	// {"Fast-SRGAN"                    ,  "image"            ,  "image"            };
	// {"RIDNet"                        ,  "input_image"      ,  "output_image"     };
	// {"pix2pixHD-Facemarks2Portrait"  ,  "image"            ,  "image"            };
	// {"BigBiGAN"                      ,  "input_image"      ,  "output_image"     };
	// {"Visual-Importance"             ,  "image"            ,  "image"            };
	// {"YOLACT"                        ,  "input_image"      ,  "output_image"     };
	// {"DensePose"                     ,  "input"            ,  "output"           };
	 {"DenseDepth"                    ,  "image"            ,  "depth_image"      };
	// {"PhotoSketch"                   ,  "input"            ,  "output"           };
//	{"Fast-Style-Transfer"           ,  "image"            ,  "output"           };
	// {"UGATIT"                        ,  "image"            ,  "image"            };
	// {"DeRaindrop"                    ,  "input_image"      ,  "output_image"     };
	// {"Automatic-Colorization"        ,  "image"            ,  "colorized_image"  };
	// {"Few-Shot-Face-Translation-GAN" ,  "source", "target" ,  "result"           };
	// {"FastPhotoStyle"                ,  "content", "style" ,  "image"            };
	// {"Liquid-Warping-GAN"            ,  "source", "target" ,  "image"            };

	
	
public:
	void setup();
	void update();
	void draw();
	
	void keyReleased(int key);
	
	void loadExtraImage();
	
	ofxRunway runway;
	
	
	ofImage runwayResult;
	
	
	ofVideoGrabber grabber;
	
	
	ofImage extraImage;
	
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);
//	bool setModel(const string & name);
	
	
};
