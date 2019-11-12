#include "ofxRunway.h"



//----------------------
ofxRunway::ofxRunway() : ofxIO::Thread(std::bind(&ofxRunway::updateThread, this)) {
	busy = true;
	state = OFX_RUNWAY_DISCONNECTED;
	ioTypesSet = false;
	
}

//----------------------
bool ofxRunway::setup(const string& host) {
	ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
	this->host = host;
	busy = false;
	
	state = OFX_RUNWAY_SETUP;
	// lookup data types for all inputs and outputs
	return getTypesLookup();
}
//----------------------
bool ofxRunway::setup(ofxRunwayListener& listenerClass, const string& host){
	listeners.unsubscribeAll();
	
	listeners.push(infoEvent.newListener(&listenerClass, &ofxRunwayListener::runwayInfoEvent));
	listeners.push(errorEvent.newListener(&listenerClass, &ofxRunwayListener::runwayErrorEvent));

	return setup(host);
}
//----------------------
bool ofxRunway::setup(ofxRunwayListener* listenerClass, const string& host){
	if(listenerClass == nullptr) return false;
	return setup(*listenerClass, host);
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
			
			for (int i=0; i<inputs.size(); i++) {
				cout << "added input : " << inputs[i]["name"] <<endl;
				inputTypes[inputs[i]["name"]].set(inputs[i]);
			}
			for (int i=0; i<outputs.size(); i++) {
				cout << "added output : " << outputs[i]["name"] << " of type: " << outputs[i]["type"] <<endl;
				outputTypes[outputs[i]["name"]].set(outputs[i]);
			}
			ioTypesSet = true;
			state = OFX_RUNWAY_CONNECTED;
			ofNotifyEvent(infoEvent, infoJson, this);
			
		}
		else
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			errorString =  response->statusAndReason();
			ofNotifyEvent(errorEvent, errorString, this);
			ofLogError("@@@ ofxRunway::getTypesLookup") << errorString;
		}
	}
	catch (const Poco::Exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		errorString =  exc.displayText();
		ofNotifyEvent(errorEvent, errorString, this);
		ofLogError("### ofxRunway::getTypesLookup") << errorString;
	}
	catch (const std::exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		errorString =  exc.what();
		ofNotifyEvent(errorEvent, errorString, this);
		ofLogError("!!! ofxRunway::getTypesLookup") << errorString;
	}
	return ioTypesSet;
}

//----------------------
void ofxRunway::send(ofxRunwayData & data){
	input.send(data);
}

//----------------------
bool ofxRunway::tryReceive(ofxRunwayData & data){
	return output.tryReceive(data);
}
//----------------------
void ofxRunway::updateThread()
{
	while (isRunning() && input.receive(dataToReceive))
	{
		if (input.size() > 1)
			continue;
		
		busy = true;
		
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

				state = OFX_RUNWAY_RUNNING;
				// send bundle back to app
				output.send(dataToSend);
			}
			else
			{
				state = OFX_RUNWAY_CONNECTION_REFUSED;
				errorString =  response->statusAndReason();
				ofNotifyEvent(errorEvent, errorString, this);
				ofLogError("@@@ ofxRunway::updateThread") << errorString;
			}
		}
		catch (const Poco::Exception& exc)
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			errorString =  exc.displayText();
			ofNotifyEvent(errorEvent, errorString, this);
			ofLogError("### ofxRunway::updateThread ") << errorString;
		}
		catch (const std::exception& exc)
		{
			errorString =  exc.what();
			ofNotifyEvent(errorEvent, errorString, this);
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			ofLogError("!!! ofxRunway::updateThread ") << errorString;
		}
		
		busy = false;
	}
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

