#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofxIO.h"


struct ofxRunwayBundle {
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

    void setup(string host, string outputKey);

    void send(ofxRunwayBundle & bundle) {
        input.send(bundle);
    }

    bool tryReceive(ofPixels & pixels) {
        return output.tryReceive(pixels);
    }

protected:

    void updateThread();

    ofxIO::ThreadChannel<ofxRunwayBundle> input;
    ofxIO::ThreadChannel<ofPixels> output;

    string host;
    string outputKey;

    int numRequests = 0;
};
