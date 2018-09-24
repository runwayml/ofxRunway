#include "ofxRunway.h"


void ofxRunway::setup(string host) {
    this->host = host;
}

void ofxRunway::updateThread() {

    ofPixels pixelsToReceive;
    ofPixels pixelsToSend;
    
    ofxIO::Base64Encoding base64Encoder;
    
    while (isRunning() && input.receive(pixelsToReceive))
    {
        if (input.size() > 1)
            continue;
        
        // Save the incoming pixels to a buffer using JPG compression.
        ofBuffer compressedPixels;
        ofSaveImage(pixelsToReceive, compressedPixels, OF_IMAGE_FORMAT_JPEG);

        // Encode the compressed pixels in base64.
        ofxIO::ByteBuffer base64CompressedPixelsIn;
        base64Encoder.encode(ofxIO::ByteBuffer(compressedPixels.getData(), compressedPixels.size()),
                             base64CompressedPixelsIn);

        // Setup the client request
        multimap<string, string> formFields = {{ "data", base64CompressedPixelsIn.toString() }};
        ofxHTTP::Client client;
        ofxHTTP::PostRequest request(host);
        request.addFormFields(formFields);
        
        // Send them to Runway server
        try
        {
            ofLog() << "execute request " << ++numRequests;
            auto response = client.execute(request);
            
            if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
            {
                // Decode base64.
                ofxIO::ByteBuffer base64CompressedPixelsOut;
                ofxIO::ByteBuffer base64CompressedPixelsProcessed(response->buffer());
                base64Encoder.decode(base64CompressedPixelsProcessed,
                                     base64CompressedPixelsOut);
                
                // Decompress JPG
                ofLoadImage(pixelsToSend, ofBuffer(base64CompressedPixelsOut.getCharPtr(),
                                                   base64CompressedPixelsOut.size()));

                // Send the pixels back to the output channel.
                output.send(pixelsToSend);
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
}

