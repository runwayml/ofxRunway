#include "ofxRunway.h"


//----------------------
void ofxRunway::setup(string host) {
    this->host = host;
    busy = false;
}

//----------------------
void ofxRunway::getTypesLookup(string address) {
    ofxHTTP::Client client;
    ofxHTTP::GetRequest request(host+"/"+address);
    ofxHTTP::Context context;

    try
    {
        auto response = client.execute(context, request);
        if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            ofLogNotice("ofApp::setup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";

            ofJson responseJson = response->json();
            ofJson inputs = responseJson["inputs"];
            ofJson outputs = responseJson["outputs"];

            for (int i=0; i<inputs.size(); i++) {
                inputTypes[inputs[i]["name"]] = inputs[i]["type"];
            }
            for (int i=0; i<outputs.size(); i++) {
                outputTypes[outputs[i]["name"]] = outputs[i]["type"];
            }
            addresses.push_back(address);
        }
        else
        {
            ofLogError("@@@ ofApp::setup") << response->getStatus() << " " << response->getReason();
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("### ofApp::setup") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("!!! ofApp::setup") << exc.what();
    }
}

//----------------------
void ofxRunway::send(ofxRunwayBundle & bundle) 
{
    input.send(bundle);
}

//----------------------
bool ofxRunway::tryReceive(ofxRunwayBundle & bundle) 
{
    return output.tryReceive(bundle);
}

//----------------------
void ofxRunway::bundleImagesToJson(ofJson & json, map<string, ofPixels> & bundleImages) 
{
    for (auto const &e1 : bundleImages) 
    {
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
}

//----------------------
template<class T> void ofxRunway::bundleToJson(ofJson & json, map<string, T> & bundle) 
{
    for (auto const &e1 : bundle) 
    {
        string key = e1.first;
        T value = e1.second;
        json[key] = value;
    }
}

//----------------------
void ofxRunway::updateThread() 
{
    while (isRunning() && input.receive(bundleToReceive))
    {
        if (input.size() > 1)
            continue;

        busy = true;

        // convert bundle to json
        ofJson json = {};
        bundleToJson(json, bundleToReceive.vectorsV);              
        bundleToJson(json, bundleToReceive.vectorsF);              
        bundleToJson(json, bundleToReceive.vectorsS);              
        bundleToJson(json, bundleToReceive.floats);        
        bundleToJson(json, bundleToReceive.ints);        
        bundleToJson(json, bundleToReceive.strings);   
        bundleImagesToJson(json, bundleToReceive.images);      

        string address = bundleToReceive.address;

        // lookup data types for all inputs and outputs
        if (find(addresses.begin(), addresses.end(), address) == addresses.end()) {
            getTypesLookup(address);
        }

        // setup json request
        ofxHTTP::Client client;
        ofxHTTP::JSONRequest request(host+"/"+address);
        request.setJSON(json);

        // Send them to Runway server
        try
        {
            auto response = client.execute(request);
            if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
            {
              	ofLogNotice("ofApp::setup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";
              	ofxRunwayBundle bundleToSend;
              	ofJson responseJson = response->json();

				for (auto it = responseJson.begin(); it != responseJson.end(); ++it) {
					string dataType = outputTypes[it.key()];

                    if (dataType == "vector") 
                    {
                        string vecType = it.value()[0].type_name();
                        if (vecType == "array") {
                            vector<vector<float> > value = it.value();
                            bundleToSend.vectorsV[it.key()] = value;
                        }
                        else if (vecType == "number") {
                            vector<float> value = it.value();
                            bundleToSend.vectorsF[it.key()] = value;
                        }
                        else if (vecType == "string") {
                            vector<string> value = it.value();
                            bundleToSend.vectorsS[it.key()] = value;
                        }
					}
                    else if (dataType == "int") 
                    {
                        int value = it.value();
                        bundleToSend.ints[it.key()] = value;
                    }
                    else if (dataType == "float") 
                    {
                        float value = it.value();
                        bundleToSend.floats[it.key()] = value;
                    }
                    else if (dataType == "string" || dataType == "text") 
                    {
                        string value = it.value();
                        bundleToSend.strings[it.key()] = value;
                    }
                    else if (dataType == "image") 
                    {
                        string imageB64 = it.value().dump();
                        imageB64 = imageB64.substr(1, imageB64.size()-2);//+"==";;
                        imageB64 = imageB64.substr(imageB64.find(",") + 1);
                        ofBuffer decodedBuffer;
                        decodedBuffer.set(ofxIO::Base64Encoding::decode(imageB64));
                        ofLoadImage(pixelsToSend, decodedBuffer);
                        bundleToSend.images[it.key()] = pixelsToSend;
                    }
				}

                // send bundle back to app
                output.send(bundleToSend);
            } 
        }
        catch (const Poco::Exception& exc)
        {
            ofLogError("ofApp::setup ") << exc.displayText();
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::setup ") << exc.what();
        }
        
        busy = false;
    }
}
