#include "ofxRunway.h"


void ofxRunway::setup(string host, string outputKey) {
    this->host = host;
    this->outputKey = outputKey;
}

void ofxRunway::updateThread() {

    ofPixels pixelsToSend;

    ofxIO::Base64Encoding base64Encoder;


    ofxRunwayBundle bundleToReceive;

    while (isRunning() && input.receive(bundleToReceive))
    {
        if (input.size() > 1)
            continue;

        ofJson json = {};

        // iterate through bundle vectors
        for (auto const &e1 : bundleToReceive.vectors) {
          string key = e1.first;
          vector<float> value = e1.second;
          json[key] = value;
        }

        // iterate through bundle floats
        for (auto const &e1 : bundleToReceive.floats) {
          string key = e1.first;
          float value = e1.second;
          json[key] = value;
        }

        // iterate through bundle integers
        for (auto const &e1 : bundleToReceive.ints) {
          string key = e1.first;
          int value = e1.second;
          json[key] = value;
        }

        // iterate through bundle strings
        for (auto const &e1 : bundleToReceive.strings) {
          string key = e1.first;
          string value = e1.second;
          json[key] = value;
        }

        // iterate through bundle images
        for (auto const &e1 : bundleToReceive.images) {
          string key = e1.first;
          ofPixels pixelsToReceive = e1.second;

          // Save the incoming pixels to a buffer using JPG compression.
          ofBuffer compressedPixels;
          ofSaveImage(pixelsToReceive, compressedPixels, OF_IMAGE_FORMAT_PNG);

          // Encode the compressed pixels in base64.
          ofxIO::ByteBuffer base64CompressedPixelsIn;
          base64Encoder.encode(ofxIO::ByteBuffer(compressedPixels.getData(), compressedPixels.size()),
                               base64CompressedPixelsIn);

          json[key] = base64CompressedPixelsIn.toString()+"==";
        }

        // setup json request
        ofxHTTP::Client client;
        ofxHTTP::JSONRequest request(host);
        request.setJSON(json);

        // Send them to Runway server
        try
        {
            ofLog() << "execute request " << ++numRequests;
            auto response = client.execute(request);
            ofLog() << " did request " << endl;

            if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
            {
              ofLogNotice("ofApp::setup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";

              // Buffer the response, or otherwise consume the stream.
              ofJson responseJson = response->json();

              //ofLogNotice("ofApp::setup") << "Content Begin";
              //std::cout << responseJson.dump(4) << std::endl;
              //ofLogNotice("ofApp::setup") << "Content End";

              // extract image
              string imageB64 = responseJson[outputKey].dump();
              imageB64 = imageB64.substr(1, imageB64.size()-2);//+"==";;
              imageB64 = imageB64.substr(imageB64.find(",") + 1);

              ofBuffer decodedBuffer;
              decodedBuffer.set(ofxIO::Base64Encoding::decode(imageB64));

              // Decompress JPG
              ofLoadImage(pixelsToSend, decodedBuffer);

              // Send the pixels back to the output channel.
              output.send(pixelsToSend);
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
