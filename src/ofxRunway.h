#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofxIO.h"


struct ofxRunwayBundle {
  string address;
  map<string, ofPixels> images;
  map<string, float> floats;
  map<string, int> ints;
  map<string, string> strings;
  map<string, vector<vector<float>> > vectorsV;
  map<string, vector<float> > vectorsF;
  map<string, vector<string> > vectorsS;
};


class ofxRunway: public ofxIO::Thread {
public:
    ofxRunway();
    virtual ~ofxRunway() {}

    void setup(string host);

    void send(ofxRunwayBundle & bundle);
    bool tryReceive(ofxRunwayBundle & bundle);

    bool getBusy() {return busy;}
        
protected:

    void getTypesLookup(string address);
    void updateThread();
    void bundleImagesToJson(ofJson & json, map<string, ofPixels> & bundleImages);
    template<class T> void bundleToJson(ofJson & json, map<string, T> & bundle);
    
    ofPixels pixelsToSend;
    ofxIO::Base64Encoding base64Encoder;
    ofxRunwayBundle bundleToReceive;
    ofxIO::ThreadChannel<ofxRunwayBundle> input;
    ofxIO::ThreadChannel<ofxRunwayBundle> output;
	map<string, string> inputTypes;
	map<string, string> outputTypes;
    vector<string> addresses;
    string host;
    bool busy;
};
