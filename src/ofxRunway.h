#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofxIO.h"

class ofxRunway: public ofxIO::Thread {
public:
    ofxRunway(): ofxIO::Thread(std::bind(&ofxRunway::updateThread, this)) {}
    virtual ~ofxRunway(){}

    void setup(string host);

    void send(ofPixels & pixels) {
        input.send(pixels);
    }
    bool tryReceive(ofPixels & pixels) {
        return output.tryReceive(pixels);
    }
    
protected:
    
    void updateThread();
    
    ofxIO::ThreadChannel<ofPixels> input;
    ofxIO::ThreadChannel<ofPixels> output;

    string host;
    
    
    int numRequests = 0;
};
