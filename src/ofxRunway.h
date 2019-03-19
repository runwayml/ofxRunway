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
  map<string, vector<float>> vectors;
};


class ofxRunway: public ofxIO::Thread {
public:
    ofxRunway(): ofxIO::Thread(std::bind(&ofxRunway::updateThread, this)) {}
    virtual ~ofxRunway(){}

    void setup(string host);

    void send(ofxRunwayBundle & bundle);
    bool tryReceive(ofxRunwayBundle & bundle);
    
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
};
