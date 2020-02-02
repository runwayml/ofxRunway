#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofxIO.h"
#include "ofxRunwayData.h"

enum ofxRunwayState{

	/// initial default state
	OFX_RUNWAY_DISCONNECTED = 0,
	/// it has been setup but no attempt of getting any data from the runway server has been done
	OFX_RUNWAY_SETUP,
	/// setup and connected to runway server but no data has been received back
	OFX_RUNWAY_CONNECTED,
	/// connected and getting data from server
	OFX_RUNWAY_RUNNING,
	/// there was an error while trying to connect to server.
	/// Probably runway is not running or the workspace in runway is not running.
	OFX_RUNWAY_CONNECTION_REFUSED
};


class ofxRunwayListener{

public:
	virtual void runwayInfoEvent(ofJson& info) = 0;
	virtual void runwayErrorEvent(string& message) = 0;
};

class ofxRunway:public  ofxIO::Thread {
public:
	
	static const ofColor green1;
	static const ofColor green2;
	static const ofColor gray;
	
	ofxRunway();
	virtual ~ofxRunway();
	
	bool setup(const string& host);
	bool setup(ofxRunwayListener& listenerClass, const string& host);
	bool setup(ofxRunwayListener* listenerClass, const string& host);

	bool send(ofxRunwayData & data);
	bool send(const string& name, const ofBaseHasPixels& img, ofxRunwayImageType type =OFX_RUNWAY_JPG, int resize_width = 0, int resize_height = 0);
	bool send(const string& name, const ofPixels& pix, ofxRunwayImageType type = OFX_RUNWAY_JPG, int resize_width = 0, int resize_height = 0);
	bool send(const string& name, string& data);
	
	
	bool tryReceive(ofxRunwayData & data);
	
	bool get(const string& name, ofImage& img);
	bool get(const string& name, ofPixels& pix);
	
	bool get(const string& name, string& data);
	
	bool get(vector<ofxRunwayCaption>& captions, float imgWidth, float imgHeight, ofxRunwayCaptionType capType);
	
	bool get(vector<ofxRunwayPose>& poses, float imgWidth, float imgHeight, ofxRunwayPoseType poseType);
	
	bool get(vector<ofxRunwayFaceLandmarks>& landmarks, float imgWidth, float imgHeight);
	
	
	bool isBusy() const; 
	
	
	ofxRunwayState getState() const;
	string getStateAsString(bool bVerbose = false) const;
	
	const string& getHost() const;
	
	
	/// \brief Draws the current status using ofBitmapFont
	/// \param x X-axis coordinate where to draw the text
	/// \param y y-axiscoordinate where to draw the text
	/// \param bVerbose if true draw the verbose (full text) for the current state.
	/// \return  the bounding box of the text drawn.
	ofRectangle drawStatus(int x = 20, int y = 20,  bool bVerbose = false) const;

	
	const ofJson& getInputType(const string& name) const;
	const ofJson& getOutputType(const string& name) const;
	
	const ofJson& getInputTypes() const;
	const ofJson& getOutputTypes() const;
	
//	const static string OFX_RUNWAY_VERSION = "0.2";
	

	ofEvent<ofJson> infoEvent;
	ofEvent<string> errorEvent;

	
	bool isServerAvailable() const;
	
	void setInfoJson(const ofJson& info);
	
	void setDataSuffixURL(const string& sufix);
	
	static string stateToString(ofxRunwayState state);
	
	
protected:

	string dataSuffix = "query";
//	string infoSuffix = "info";
	
	bool getTypesLookup();
	void updateThread();
	
	ofPixels pixelsToSend;
	
	ofxRunwayData dataToReceive;
	
	ofxIO::ThreadChannel<ofxRunwayData> input;
	ofxIO::ThreadChannel<ofxRunwayData> output;
	
	ofJson inputTypes;
	ofJson outputTypes;

	string host;
	int    port;

	std::atomic<bool> busy;
	
	enum ofxRunwayTypeStatus{
		OFX_RUNWAY_TYPE_NOT_SET = 0,
		OFX_RUNWAY_TYPE_WAITING,
		OFX_RUNWAY_TYPE_SET
	};
	
	
	std::atomic<ofxRunwayTypeStatus> ioTypesSet;
	
	ofJson infoJson;
	
	std::atomic<ofxRunwayState> state;

	ofEventListeners listeners;
	
	
	std::string errorString;

	virtual void onExit() override;
	
	void closeChannels();
private:
	void exitCB(ofEventArgs&);
	
	ofEventListener exitListener;
	
	enum CallbackType {
		CALLBACK_INFO,
		CALLBACK_DATA
	};
	
	enum ofxRunwayRequestType{
		OFX_RUNWAY_REQUEST_GET,
		OFX_RUNWAY_REQUEST_POST,
		OFX_RUNWAY_REQUEST_DELETE
	};


	
	void makeRequest(const string& address,  CallbackType callbackType, const string & funcName, const ofJson& requestData = ofJson());
	void makeRequest(const string& address,  std::function<void (const ofJson& data)> callback , const ofJson& requestData, ofxRunwayRequestType requestType, const string & funcName );
	
	void requestInfoCallback(const ofJson& info);
	void requestDataCallback(const ofJson& data);

	void notifyError(const string & funcName, const string& errorMsg);
	ofBitmapFont bf;
};


