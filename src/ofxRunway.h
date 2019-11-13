#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofxIO.h"
#include "ofxRunwayData.h"


struct ofxRunwayIOInfo{
//	ofxRunwayIOInfo(const string& _name,const string& _type,const string& _description):name(_name),type(_type),description(_description){}
	string name;
	string type;
	string description;
	ofJson json;
	void set(const ofJson& json){
		this->json = json;
		name = json["name"];
		type = json["type"];
		string descType = json["description"].type_name();
		if( descType == "string"){
			description = json["description"];
		}
		
	}
	
	friend std::ostream & operator<<(std::ostream & ostr, const ofxRunwayIOInfo & info){
		
		ostr << "name: " << info.name << "  type: " << info.type;
		if(!info.description.empty())
		ostr << "  description: " << info.description;
		ostr << endl;
		return ostr;
	}
};
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
	ofxRunway();
	virtual ~ofxRunway() {}
	
	bool setup(const string& host);
	bool setup(ofxRunwayListener& listenerClass, const string& host);
	bool setup(ofxRunwayListener* listenerClass, const string& host);

	void send(ofxRunwayData & data);
	bool tryReceive(ofxRunwayData & data);
	
	bool send(const string& name, const ofBaseHasPixels& img, ofxRunwayImageType type =OFX_RUNWAY_JPG);
	bool send(const string& name, const ofPixels& pix, ofxRunwayImageType type = OFX_RUNWAY_JPG);
	
	bool get(const string& name, ofImage& img);
	bool get(const string& name, ofPixels& pix);
	
	bool isBusy() {return busy;}
	
	
	ofxRunwayState getState();
	string getStateAsString(bool bVerbose = false);
	
	const string& getHost();
	
	
	/// \brief Draws the current status using ofBitmapFont
	/// \param x X-axis coordinate where to draw the text
	/// \param y y-axiscoordinate where to draw the text
	/// \param bVerbose if true draw the verbose (full text) for the current state.
	/// \return  the bounding box of the text drawn.
	ofRectangle drawStatus(int x = 20, int y = 20,  bool bVerbose = false);

	
	const ofxRunwayIOInfo& getInputType(const string& name);
	const ofxRunwayIOInfo& getOutputType(const string& name);
	
	const map<string, ofxRunwayIOInfo>& getInputTypes();
	const map<string, ofxRunwayIOInfo>& getOutputTypes();
	
//	const static string OFX_RUNWAY_VERSION = "0.2";
	

	ofEvent<ofJson> infoEvent;
	ofEvent<string> errorEvent;
	
	
protected:

	
	bool getTypesLookup();
	void updateThread();
	
	ofPixels pixelsToSend;
	
	ofxRunwayData dataToReceive;
	
	ofxIO::ThreadChannel<ofxRunwayData> input;
	ofxIO::ThreadChannel<ofxRunwayData> output;
	
	map<string, ofxRunwayIOInfo> inputTypes;
	map<string, ofxRunwayIOInfo> outputTypes;

	string host;
	int    port;

	std::atomic<bool> busy, ioTypesSet;
	
	ofJson infoJson;

	std::atomic<ofxRunwayState> state;

	ofEventListeners listeners;
	
	
	std::string errorString;
	
private:
	enum RequestType{
		REQUEST_INFO,
		REQUEST_DATA
	};
	
	void makeRequest(const string& address,  RequestType requestType, const ofJson& requestData = ofJson());
	
	void requestInfoCallback(const ofJson& info);
	void requestDataCallback(const ofJson& data);
	
};


