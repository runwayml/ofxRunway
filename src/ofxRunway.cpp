#include "ofxRunway.h"
#include "ofxHTTP.h"


const ofColor ofxRunway::green1 = {115, 229, 162, 255};
const ofColor ofxRunway::green2 = { 51, 178, 121, 255};
const ofColor ofxRunway::gray   = { 28,  28,  28, 255};

//----------------------
ofxRunway::ofxRunway() : ofxIO::Thread(std::bind(&ofxRunway::updateThread, this)) {
	exitListener = ofEvents().exit.newListener(this, &ofxRunway::exitCB);
	busy = true;
	state = OFX_RUNWAY_DISCONNECTED;
	ioTypesSet = OFX_RUNWAY_TYPE_NOT_SET;
}
ofxRunway::~ofxRunway(){
	closeChannels();
}
//----------------------
void ofxRunway::closeChannels(){
	input.close();
	output.close();
}
//----------------------
void ofxRunway::onExit(){
	closeChannels();
}
//----------------------
void ofxRunway::exitCB(ofEventArgs&){
	closeChannels();
}
//----------------------
bool ofxRunway::setup(const string& host) {
//	ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
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
	
	ioTypesSet = OFX_RUNWAY_TYPE_SET;
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
void ofxRunway::notifyError(const string & funcName, const string& errorMsg){
	if(ioTypesSet == OFX_RUNWAY_TYPE_WAITING){
		ioTypesSet = OFX_RUNWAY_TYPE_NOT_SET;
	}
	errorString = host +"/"+dataSuffix+ " : " + funcName + " -> " + errorMsg;
	ofNotifyEvent(errorEvent, errorString, this);
	ofLogError("ofxRunway::"+funcName) << errorString;
}
//----------------------
void ofxRunway::makeRequest(const string& address,  std::function<void (const ofJson& data)> callback , const ofJson& requestData, ofxRunwayRequestType requestType, const string & funcName ){
	ofxHTTP::Client client;
	std::unique_ptr<ofxHTTP::Request> request;
	ofxHTTP::Context context;
	
	if(requestType == OFX_RUNWAY_REQUEST_GET){
		request = make_unique<ofxHTTP::GetRequest>(address);
	}else if(requestType == OFX_RUNWAY_REQUEST_POST){
		request = make_unique<ofxHTTP::JSONRequest>(address, requestData, Poco::Net::HTTPMessage::HTTP_1_1);
	}else if(requestType == OFX_RUNWAY_REQUEST_DELETE){
		request = make_unique<ofxHTTP::FormRequest>(Poco::Net::HTTPRequest::HTTP_DELETE, address, Poco::Net::HTTPMessage::HTTP_1_1);
	}
	request->setContentType("application/json");
	request->add("Accept","application/json");
	request->set("Accept-Encoding","");
	try
	{
		
		auto response = client.execute(context, *request);
		if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
		{
			if(requestType == OFX_RUNWAY_REQUEST_DELETE){
				if(response->json().count("removed") && response->json()["removed"].get<bool>()){
						callback(requestData);
				}
			}else{
				callback(response->json());
			}
		}
		else
		{
			state = OFX_RUNWAY_CONNECTION_REFUSED;
			notifyError(funcName , "Http Response Error : " + response->statusAndReason());
		}
	}
	catch (const Poco::Exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		notifyError(funcName, "Poco:exception : " + exc.displayText());
	}
	catch (const std::exception& exc)
	{
		state = OFX_RUNWAY_CONNECTION_REFUSED;
		notifyError(funcName, "std:exception : " + (string)exc.what());
	}
}
void ofxRunway::makeRequest(const string& address,  CallbackType callbackType, const string & funcName,const ofJson& requestData){
	if(callbackType == CALLBACK_INFO){
		makeRequest(address, std::bind(&ofxRunway::requestInfoCallback, this, std::placeholders::_1), requestData, OFX_RUNWAY_REQUEST_GET, funcName);
	}else if(callbackType == CALLBACK_DATA){
		makeRequest(address, std::bind(&ofxRunway::requestDataCallback, this, std::placeholders::_1), requestData, OFX_RUNWAY_REQUEST_POST, funcName);
	}
}
//----------------------
bool ofxRunway::getTypesLookup() {
	if(ioTypesSet == OFX_RUNWAY_TYPE_SET){
		return true;
	}
	ioTypesSet == OFX_RUNWAY_TYPE_WAITING;
	makeRequest(host+"/info", CALLBACK_INFO, "getInfo");

	return ioTypesSet == OFX_RUNWAY_TYPE_SET;
}

//----------------------
bool ofxRunway::send(ofxRunwayData & data){
	return input.send(data);
}

//----------------------
bool ofxRunway::tryReceive(ofxRunwayData & data){
	return output.tryReceive(data);
}
//----------------------
void ofxRunway::updateThread()
{
	while (isRunning()){
		if(ioTypesSet == OFX_RUNWAY_TYPE_NOT_SET){
			getTypesLookup();
		}
		if(input.receive(dataToReceive)){
//			if (input.size() > 1)
//				continue;
			busy = true;

			makeRequest(host+"/"+dataSuffix,CALLBACK_DATA, "query", dataToReceive.data);

			busy = false;
		}
	}
}
//----------------------
ofxRunwayState ofxRunway::getState() const{
	return state;
}
//----------------------
const string& ofxRunway::getHost() const{
	return host;
}
//----------------------
ofRectangle ofxRunway::drawStatus(int x , int y, bool bVerbose ) const{
	
	ofColor backgroundColor = ofColor::black;
	if(getState() == OFX_RUNWAY_CONNECTION_REFUSED){
		backgroundColor = ofColor(255, 0, 0, ofMap(sin(ofGetElapsedTimef()*3), -1, 1, 100, 255));
	}
	auto s = getStateAsString(bVerbose);
	ofDrawBitmapStringHighlight(s, x, y, backgroundColor);
	
	return bf.getBoundingBox(s, x, y);
//	return ofRectangle(x,y, 0,0);
	
}
//----------------------
string ofxRunway::getStateAsString(bool bVerbose) const{
	stringstream ss;
	
//	ofxRunwayTypeStatus ioSet = ioTypesSet;
//	if (ioSet == OFX_RUNWAY_TYPE_NOT_SET){
//		ss <<"OFX_RUNWAY_TYPE_NOT_SET" <<endl;
//	}else if (ioSet == OFX_RUNWAY_TYPE_WAITING){
//		ss <<"OFX_RUNWAY_TYPE_WAITING" << endl;
//	}else if (ioSet == OFX_RUNWAY_TYPE_SET){
//		ss <<"OFX_RUNWAY_TYPE_SET" << endl;
//	}
	auto st = getState();
	ss << "ofxRunway : host "<< host << endl << stateToString(st);
	if(bVerbose){
		if(st == OFX_RUNWAY_DISCONNECTED){
			ss << endl <<  "Initial default state";
		}
		else if(st == OFX_RUNWAY_SETUP){
			ss << endl <<  "It has been setup but no attempt of getting any data from the runway server has been done";
		}
		else if(st == OFX_RUNWAY_CONNECTED){
			ss << endl <<  "Setup and connected to runway server but no data has been received back";
		}
		else if(st == OFX_RUNWAY_RUNNING){
			ss << endl <<  "Connected and getting data from server";
		}
	}
	if(st == OFX_RUNWAY_CONNECTION_REFUSED){
		ss << endl;
		ss << "    There was an error while trying to connect to runway server "  << endl;
		ss << "    Probably runway is not running or the workspace in runway is not running." << endl;
		ss << "    Aswell make sure that you have the DENSE CAP model in your workspace.";
	
	}
	return ss.str();
}
//----------------------
const ofJson& ofxRunway::getInputTypes() const{
	return inputTypes;
}
//----------------------
const ofJson& ofxRunway::getOutputTypes() const{
	return outputTypes;
}
//----------------------
const ofJson& getType(const string& name, const ofJson& types) {
	if(types.count(name)){
		return types[name];
	}
	static ofJson dummyInfo;
	return dummyInfo;
}
//----------------------
const ofJson& ofxRunway::getInputType(const string& name) const{
	return getType(name, inputTypes);
}
//----------------------
const ofJson& ofxRunway::getOutputType(const string& name) const{
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
	
	return send(data);
	
}
//----------------------
bool ofxRunway::send(const string& name, string& data){
	if(isBusy()) return false;
	
	ofxRunwayData runWayData;
	runWayData.setString(name, data);
	
	return send(runWayData);

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
bool ofxRunway::isServerAvailable() const{
	return (state != OFX_RUNWAY_DISCONNECTED && state != OFX_RUNWAY_CONNECTION_REFUSED);
}
//----------------------
bool ofxRunway::isBusy() const{
	return busy;
}
//----------------------
void ofxRunway::setInfoJson(const ofJson& info){
	requestInfoCallback(info);
}
//----------------------
void ofxRunway::setDataSuffixURL(const string& sufix){
	dataSuffix = sufix;
}
//----------------------
string ofxRunway::stateToString(ofxRunwayState state){
	switch (state) {
		case OFX_RUNWAY_DISCONNECTED : return "DISCONNECTED";
		case OFX_RUNWAY_SETUP : return "SETUP";
		case OFX_RUNWAY_CONNECTED : return "CONNECTED";
		case OFX_RUNWAY_RUNNING : return "RUNNING";
		case OFX_RUNWAY_CONNECTION_REFUSED : return "CONNECTION REFUSED";
	}
	return "";
}
