#include "ofxRunway.h"
#include "ofxHTTP.h"


const ofColor ofxRunway::green1 = {115, 229, 162, 255};
const ofColor ofxRunway::green2 = { 51, 178, 121, 255};
const ofColor ofxRunway::gray   = { 28,  28,  28, 255};

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
void ofxRunway::requestInfoCallback(const ofJson& info){
	infoJson = info;
	
	inputTypes = infoJson["inputs"];
	outputTypes = infoJson["outputs"];
	
	ioTypesSet = true;
	state = OFX_RUNWAY_CONNECTED;
	ofNotifyEvent(infoEvent, infoJson, this);
}
//----------------------
void ofxRunway::requestDataCallback(const ofJson& data){

	ofxRunwayData dataToSend;
	dataToSend.data = data;
	
	state = OFX_RUNWAY_RUNNING;
	
	output.send(dataToSend);
}
//----------------------
void ofxRunway::makeRequest(const string& address,  RequestType requestType,const ofJson& requestData){
	ofxHTTP::Client client;
	std::unique_ptr<ofxHTTP::Request> request;
	ofxHTTP::Context context;
	
	if(requestType == REQUEST_INFO){
		request = make_unique<ofxHTTP::GetRequest>(address);
	}else{
		request = make_unique<ofxHTTP::JSONRequest>(address, requestData, Poco::Net::HTTPMessage::HTTP_1_1);
	}
	string funcName = (string)((requestType == REQUEST_INFO)?"request info": "request data");
	
	try
	{
		auto response = client.execute(context, *request);
		if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
		{
			if(requestType == REQUEST_INFO){
				requestInfoCallback(response->json());
			}else if(requestType == REQUEST_DATA){
				requestDataCallback(response->json());
			}
		}
		else
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			errorString =  response->statusAndReason();
			errorString = funcName + " -> " + errorString;
			ofNotifyEvent(errorEvent, errorString, this);
			ofLogError("@@@ ofxRunway::"+funcName) << errorString;
		}
	}
	catch (const Poco::Exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		errorString =  exc.displayText();
		errorString = funcName + " -> " + errorString;
		ofNotifyEvent(errorEvent, errorString, this);
		ofLogError("### ofxRunway::"+funcName) << errorString;
	}
	catch (const std::exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		errorString = exc.what();
		errorString = funcName + " -> " + errorString;
		ofNotifyEvent(errorEvent, errorString, this);
		ofLogError("!!! ofxRunway::"+funcName) << errorString;
	}

}
//----------------------
bool ofxRunway::getTypesLookup() {
	if(ioTypesSet){
		return true;
	}
	makeRequest(host+"/info", REQUEST_INFO);

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
		
		makeRequest(host+"/query",REQUEST_DATA, dataToReceive.data);

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
const ofJson& ofxRunway::getInputTypes(){
	return inputTypes;
}
//----------------------
const ofJson& ofxRunway::getOutputTypes(){
	return outputTypes;
}
//----------------------
const ofJson& getType(const string& name, const ofJson& types){
	if(types.count(name)){
		return types[name];
	}
	static ofJson dummyInfo;
	return dummyInfo;
}
//----------------------
const ofJson& ofxRunway::getInputType(const string& name){
	return getType(name, inputTypes);
}
//----------------------
const ofJson& ofxRunway::getOutputType(const string& name){
	return getType(name, outputTypes);
}
//----------------------
bool ofxRunway::get(const string& name, ofImage& img){
	if(get(name, img.getPixels())){
		img.update();
		return true;
	}
	return false;
}
//----------------------
bool ofxRunway::get(const string& name, ofPixels& pix){
	ofxRunwayData dataToReceive;
	while (tryReceive(dataToReceive)) {
		if(dataToReceive.getImage(name, pix)){
			return true;
		}
	}
	return false;
}
//----------------------
bool ofxRunway::get(vector<ofxRunwayCaption>& captions, float imgWidth, float imgHeight, ofxRunwayCaptionType capType){
	ofxRunwayData dataToReceive;
	while (tryReceive(dataToReceive)) {
		return dataToReceive.getCaptions(captions, imgWidth, imgHeight, capType);
	}
	return false;
}
//----------------------
bool ofxRunway::get(vector<ofxRunwayPose>& poses, float imgWidth, float imgHeight, ofxRunwayPoseType poseType){
	ofxRunwayData dataToReceive;
	while (tryReceive(dataToReceive)) {
		return dataToReceive.getPoses(poses, imgWidth, imgHeight, poseType);
	}
	return false;
}
//----------------------
bool ofxRunway::get(vector<ofxRunwayFaceLandmarks>& landmarks, float imgWidth, float imgHeight){
	ofxRunwayData dataToReceive;
	while (tryReceive(dataToReceive)) {
		return dataToReceive.getFaceLandmarks(landmarks, imgWidth, imgHeight);
	}
	return false;
}
//----------------------
bool ofxRunway::send(const string& name, const ofBaseHasPixels& img, ofxRunwayImageType type, int resize_width, int resize_height){
	return send(name, img.getPixels(), type, resize_width, resize_height);
}
//----------------------
bool ofxRunway::send(const string& name, const ofPixels& pix, ofxRunwayImageType type, int resize_width, int resize_height){
	if(isBusy()) return false;
	
	ofxRunwayData data;
	data.setImage(name, pix, type, resize_width, resize_height);
	
	send(data);
	return true;
}
//----------------------
bool ofxRunway::send(const string& name, string& data){
	if(isBusy()) return false;
	
	ofxRunwayData runWayData;
	runWayData.setString(name, data);
	
	send(runWayData);
	return true;

}
//----------------------
bool ofxRunway::get(const string& name, string& data){
	ofxRunwayData dataToReceive;
	while (tryReceive(dataToReceive)) {
		return dataToReceive.getString(name, data);
	}
	return false;
}

//----------------------
bool ofxRunway::isServerAvailable(){
	return (state != OFX_RUNWAY_DISCONNECTED && state != OFX_RUNWAY_CONNECTION_REFUSED);
	
}





