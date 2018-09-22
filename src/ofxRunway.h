#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofxIO.h"

class ofxRunway: public ofxIO::Thread {
public:
    ofxRunway(): ofxIO::Thread(std::bind(&ofxRunway::updateThread, this)) {}
    virtual ~ofxRunway(){}

    ofxIO::ByteBuffer & getOutputBuffer() {return decodedBuffer;}
    
    void setup(string host);
    void send(ofImage & imgSend);

    static ofEvent<void> runwayEvent;
    
protected:
    
    void updateThread();
    void sendHelper();
    
    string host;
    bool toSend;
    
    ofxHTTP::Client client;
    ofBuffer bufferIn;
    ofxIO::ByteBuffer decodedBuffer;
    ofImage imgSend;
    
    // ofImage imgReceive;
};
