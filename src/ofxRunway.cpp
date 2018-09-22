#include "ofxRunway.h"


ofEvent<void> ofxRunway::runwayEvent = ofEvent<void>();

void ofxRunway::setup(string host) {
    this->host = host;
}

void ofxRunway::send(ofImage & imgSend) {
    this->imgSend = imgSend;
    toSend = true;
}

void ofxRunway::updateThread() {
    while (ofxIO::Thread::isRunning()) {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait_for(lock, std::chrono::milliseconds(1));
        if (toSend) {
            sendHelper();
            toSend = false;
        }
    }
}

void ofxRunway::sendHelper() {
    ofSaveImage(imgSend.getPixels(), bufferIn, OF_IMAGE_FORMAT_JPEG);
    multimap<string, string> formFields = {{ "data", ofxIO::Base64Encoding::encode(bufferIn) }};
    
    // create request
    ofxHTTP::PostRequest request(host);
    request.addFormFields(formFields);
    
    try
    {
        ofLog() << "execute request";
        auto response = client.execute(request);
        
        if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            ofxIO::ByteBuffer encodedBuffer(response->buffer());
            ofxIO::Base64Encoding encoding;
            encoding.decode(encodedBuffer, decodedBuffer);

            //ofPixels pixelsOut;
            //ofLoadImage(pixelsOut, ofBuffer(decodedBuffer.getCharPtr(), decodedBuffer.size()));
            //imgReceive.setFromPixels(pixelsOut);  // <-- this was crashing before

            ofNotifyEvent(runwayEvent);
        }
        else
        {
            ofLogError("ofApp::setup") << response->getStatus() << " " << response->getReason();
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("ofApp::setup") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::setup") << exc.what();
    }
}

