#include "ofxRunway.h"



//----------------------
ofxRunway::ofxRunway() : ofxIO::Thread(std::bind(&ofxRunway::updateThread, this)) {
	busy = true;
	imageDownscaling = 0.25;
	state = OFX_RUNWAY_DISCONNECTED;
	ioTypesSet = false;
	
}

//----------------------
bool ofxRunway::setup(const string& host) {
	this->host = host;
	busy = false;
	
	state = OFX_RUNWAY_SETUP;
	// lookup data types for all inputs and outputs
	return getTypesLookup();
}

//----------------------
bool ofxRunway::getTypesLookup() {
	if(ioTypesSet){
		return true;
	}
	ofxHTTP::Client client;
	ofxHTTP::GetRequest request(host+"/info");
	ofxHTTP::Context context;
	
	try
	{
		auto response = client.execute(context, request);
		if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
		{
			ofLogVerbose("ofxRunway::getTypesLookup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";
			
			
			ofJson responseJson = response->json();
			infoJson = responseJson;
			
			ofJson inputs = responseJson["inputs"];
			ofJson outputs = responseJson["outputs"];
			//
			//			cout << "-----------------------------" << endl;
			//			cout << "            INPUTS           " << endl;
			//			cout << "-----------------------------" << endl;
			//			cout << inputs.dump(4) <<endl;
			//
			//			cout << "-----------------------------" << endl;
			//			cout << "           OUTPUTS           " << endl;
			//			cout << "-----------------------------" << endl;
			//			cout << outputs.dump(4) <<endl;
			//
			
			for (int i=0; i<inputs.size(); i++) {
				cout << "added input : " << inputs[i]["name"] <<endl;
				inputTypes[inputs[i]["name"]].set(inputs[i]);
			}
			for (int i=0; i<outputs.size(); i++) {
				cout << "added output : " << outputs[i]["name"] << " of type: " << outputs[i]["type"] <<endl;
				outputTypes[outputs[i]["name"]].set(outputs[i]);
			}
			//			addresses.push_back(address);
			//			for(auto& i: inputTypes){
			//				cout << i.second;
			//			}
			//			for(auto& o: outputTypes){
			//				cout << o.second;
			//			}
			ioTypesSet = true;
			state = OFX_RUNWAY_CONNECTED;
			
		}
		else
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			ofLogError("@@@ ofxRunway::getTypesLookup") << response->getStatus() << " " << response->getReason();
		}
	}
	catch (const Poco::Exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		ofLogError("### ofxRunway::getTypesLookup") << exc.displayText();
	}
	catch (const std::exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		ofLogError("!!! ofxRunway::getTypesLookup") << exc.what();
	}
	return ioTypesSet;
}

//----------------------
void ofxRunway::send(ofxRunwayData & data)
{
	input.send(data);
}

//----------------------
bool ofxRunway::tryReceive(ofxRunwayData & data)
{
	return output.tryReceive(data);
}


//----------------------
//void ofxRunway::bundleImagesToJson(ofJson & json, map<string, ofPixels> & bundleImages)
//{
//	for (auto const &e1 : bundleImages)
//	{
//		string key = e1.first;
//		ofPixels pixelsToReceive = e1.second;
//		//		cout << "ofxRunway::bundleImagesToJson : Width: " <<  pixelsToReceive.getWidth() << "  height: " << pixelsToReceive.getHeight() << endl;
//		
//		pixelsToReceive.resize(pixelsToReceive.getWidth() * imageDownscaling, pixelsToReceive.getHeight() * imageDownscaling);
//		// Save the incoming pixels to a buffer using JPG compression.
//		ofBuffer compressedPixels;
//		ofSaveImage(pixelsToReceive, compressedPixels, OF_IMAGE_FORMAT_JPEG);
//		
//		// Encode the compressed pixels in base64.
//		ofxIO::ByteBuffer base64CompressedPixelsIn;
//		base64Encoder.encode(ofxIO::ByteBuffer(compressedPixels.getData(), compressedPixels.size()),
//							 base64CompressedPixelsIn);
//		
//		json[key] = "data:image/jpeg;base64,"+base64CompressedPixelsIn.toString()+"==";
//	}
//}

////----------------------
//template<class T> void ofxRunway::bundleToJson(ofJson & json, map<string, T> & bundle)
//{
//	for (auto const &e1 : bundle)
//	{
//		//		string key = e1.first;
//		//		T value = e1.second;
//		//		json[key] = value;
//		json[e1.first] = e1.second;
//		
//	}
//}

//void ofxRunway::jsonToBundle(ofxRunwayBundle &bundle, const ofJson& json){
//	for (auto it = json.begin(); it != json.end(); ++it) {
//		string dataType = outputTypes[it.key()].type;
//		
//		if (dataType == "array")
//		{
//			string vecType = it.value()[0].type_name();
//			if (vecType == "array") {
//				vector<vector<float> > value = it.value();
//				bundle.vectorsV[it.key()] = value;
//			}
//			else if (vecType == "number") {
//				vector<float> value = it.value();
//				bundle.vectorsF[it.key()] = value;
//			}
//			else if (vecType == "string") {
//				vector<string> value = it.value();
//				bundle.vectorsS[it.key()] = value;
//			}
//		}
//		else if (dataType == "int")
//		{
//			int value = it.value();
//			bundle.ints[it.key()] = value;
//		}
//		else if (dataType == "float")
//		{
//			float value = it.value();
//			bundle.floats[it.key()] = value;
//		}
//		else if (dataType == "string" || dataType == "text")
//		{
//			string value = it.value();
//			bundle.strings[it.key()] = value;
//		}
//		else if (dataType == "image")
//		{
//			string imageB64 = it.value().dump();
//			imageB64 = imageB64.substr(1, imageB64.size()-2);//+"==";;
//			imageB64 = imageB64.substr(imageB64.find(",") + 1);
//			ofBuffer decodedBuffer;
//			decodedBuffer.set(ofxIO::Base64Encoding::decode(imageB64));
//			
//			ofLoadImage(bundle.images[it.key()], decodedBuffer);
//			//			bundle.images[it.key()] = pixelsToSend;
//		}
//	}
//}

//----------------------
void ofxRunway::updateThread()
{
	while (isRunning() && input.receive(dataToReceive))
	{
		if (input.size() > 1)
			continue;
		
		busy = true;
		
		// convert bundle to json
//		ofJson json = {};
//		bundleToJson(json, bundleToReceive.vectorsV);
//		bundleToJson(json, bundleToReceive.vectorsF);
//		bundleToJson(json, bundleToReceive.vectorsS);
//		bundleToJson(json, bundleToReceive.floats);
//		bundleToJson(json, bundleToReceive.ints);
//		bundleToJson(json, bundleToReceive.strings);
//		bundleImagesToJson(json, bundleToReceive.images);
//
		
		if(!ioTypesSet) getTypesLookup();
		
		// setup json request
		ofxHTTP::Client client;
		ofxHTTP::JSONRequest request(host+"/query");
		request.setJSON(dataToReceive.data);
		// Send them to Runway server
		try
		{
			auto response = client.execute(request);
			if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				
				ofLogVerbose("ofxRunway::updateThread") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";
				ofxRunwayData dataToSend;
				dataToSend.data = response->json();
//				jsonToBundle(bundleToSend,responseJson );
				
				state = OFX_RUNWAY_RUNNING;
				// send bundle back to app
				output.send(dataToSend);
			}
		}
		catch (const Poco::Exception& exc)
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			ofLogError("### ofxRunway::updateThread ") << exc.displayText();
		}
		catch (const std::exception& exc)
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			ofLogError("!!! ofxRunway::updateThread ") << exc.what();
		}
		
		busy = false;
	}
}
//----------------------
void ofxRunway::setImageDownscaling(float downscaling){
	imageDownscaling = downscaling;
}
//----------------------
float ofxRunway::getImageDownscaling(){
	return imageDownscaling;
}
//----------------------
ofxRunwayState ofxRunway::getState(){
	return state;
}
//----------------------
const string& ofxRunway::getHost(){
	return host;
}
//----------------------
ofRectangle ofxRunway::drawStatus(int x , int y, bool bVerbose ){
	
	
	ofColor backgroundColor = ofColor::black;
	if(getState() == OFX_RUNWAY_CONNECTION_REFUSED){
		backgroundColor = ofColor(255, 0, 0, ofMap(sin(ofGetElapsedTimef()*3), -1, 1, 100, 255));
	}
	auto s = getStateAsString(bVerbose);
	ofDrawBitmapStringHighlight(s, x, y, backgroundColor);
	ofBitmapFont bf;
	return bf.getBoundingBox(s, x, y);
	
}
//----------------------
string ofxRunway::getStateAsString(bool bVerbose){
	stringstream ss;
	auto st = getState();
	if(st == OFX_RUNWAY_DISCONNECTED){
		ss << "ofxRunway : host "<< host << " DISCONNECTED";
		if(bVerbose) ss << endl <<  "Initial default state";
	}
	else if(st == OFX_RUNWAY_SETUP){
		ss << "ofxRunway : host "<< host << " SETUP";
		if(bVerbose) ss << endl <<  "It has been setup but no attempt of getting any data from the runway server has been done";
	}
	else if(st == OFX_RUNWAY_CONNECTED){
		ss << "ofxRunway : host "<< host << " CONNECTED";
		if(bVerbose) ss << endl <<  "Setup and connected to runway server but no data has been received back";
	}
	else if(st == OFX_RUNWAY_RUNNING){
		ss << "ofxRunway : host "<< host << " RUNNING";
		if(bVerbose) ss << endl <<  "Connected and getting data from server";
	}
	else if(st == OFX_RUNWAY_CONNECTION_REFUSED){
		ss << "ofxRunway : host "<< host << " CONNECTION REFUSED" << endl;
		ss << "    There was an error while trying to connect to runway server "  << endl;
		ss << "    Probably runway is not running or the workspace in runway is not running." << endl;
		ss << "    Aswell make sure that you have the DENSE CAP model in your workspace.";
	}
	return ss.str();
}
//----------------------
const map<string, ofxRunwayIOInfo>& ofxRunway::getInputTypes(){
	return inputTypes;
}
//----------------------
const map<string, ofxRunwayIOInfo>& ofxRunway::getOutputTypes(){
	return outputTypes;
}
//----------------------
const ofxRunwayIOInfo& getType(const string& name, map<string, ofxRunwayIOInfo>& types){
	if(types.count(name)){
		return types[name];
	}
	static ofxRunwayIOInfo dummyInfo;
	return dummyInfo;
}
//----------------------
const ofxRunwayIOInfo& ofxRunway::getInputType(const string& name){
	return getType(name, inputTypes);
}
//----------------------
const ofxRunwayIOInfo& ofxRunway::getOutputType(const string& name){
	return getType(name, outputTypes);
}

