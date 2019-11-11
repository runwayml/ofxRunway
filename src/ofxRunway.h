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


class ofxRunway:public  ofxIO::Thread {
public:
	ofxRunway();
	virtual ~ofxRunway() {}
	
	bool setup(const string& host);
	
	void send(ofxRunwayData & data);
	bool tryReceive(ofxRunwayData & data);
	
	bool isBusy() {return busy;}
	
	void setImageDownscaling(float downscaling);// default 0.25
	float getImageDownscaling();
	
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
	
	// default host is localhost
//	const static string DEFAULT_HOST = "localhost";
//
//	/**
//	 * shorthand for drawing PoseNet parts into the sketch's default graphics buffer
//	 * @param data - the ofJson received from Runway
//	 * @param ellipseSize - how large should joints be rendered
//	 */
//	void drawPoseNetParts(ofJson data,float ellipseSize){
//		ModelUtils.drawPoseParts(data, parent.g, ellipseSize);
//	}
//	
//	
	
	
protected:
		
//	
//	
//	/**
//	 * if <pre>runwayInfoEvent</pre> is present it calls it passing the info <pre>ofJson</pre>
//	 * @param info
//	 */
//	void dispatchInfo(ofJson info){
//		// if the callback isn't null
//		if (onInfoEventMethod != null) {
//			// try to call it
//			try {
//				// JSON parse first string argument and pass as callback argument
//				onInfoEventMethod.invoke(parent, info);
//			}catch (Exception e) {
//				System.err.println("Error, disabling runwayInfoEvent()");
//				System.err.println(e.getLocalizedMessage());
//				onInfoEventMethod = null;
//			}
//		}
//	}
//	
//	/**
//	 * if <pre>runwayErrorEvent</pre> is present it calls it passing the error string
//	 * @param info
//	 */
//	void dispatchError(const string&  message){
//		// if the callback isn't null
//		if (onErrorEventMethod != null) {
//			// try to call it
//			try {
//				// pass OSC first argument as callback argument
//				onErrorEventMethod.invoke(parent, message);
//			}catch (Exception e) {
//				System.err.println("Error, disabling runwayErrorEvent()");
//				System.err.println(e.getLocalizedMessage());
//				onErrorEventMethod = null;
//			}
//		}
//	}
//	
//	/**
//	 * if <pre>runwayDataEvent</pre> is present it calls it passing inference data
//	 * @param data
//	 */
//	void dispatchData(ofJson data){
//		// if the callback isn't null
//		if (onDataEventMethod != null) {
//			// try to call it
//			try {
//				// JSON parse first string argument and pass as callback argument
//				onDataEventMethod.invoke(parent, data);
//			}catch (Exception e) {
//				System.err.println("Error, disabling runwayDataEvent()");
//				System.err.println(e.getLocalizedMessage());
//				onDataEventMethod = null;
//			}
//		}
//	}
//	



	
	
	
	//reference: https://stackoverflow.com/Questions/5667371/validate-ipv4-address-in-java
	// const static Pattern IPV4_PATTERN = Pattern.compile("^(([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");
	
//	void jsonToBundle(ofxRunwayBundle &bundleToSend, const ofJson& json);
	
	bool getTypesLookup();
	void updateThread();
//	void bundleImagesToJson(ofJson & json, map<string, ofPixels> & bundleImages);
//	template<class T> void bundleToJson(ofJson & json, map<string, T> & bundle);
	
	ofPixels pixelsToSend;
	
//	ofxIO::Base64Encoding base64Encoder;
	ofxRunwayData dataToReceive;
	
	ofxIO::ThreadChannel<ofxRunwayData> input;
	ofxIO::ThreadChannel<ofxRunwayData> output;
	
	map<string, ofxRunwayIOInfo> inputTypes;
	map<string, ofxRunwayIOInfo> outputTypes;

	
	string host;
	int    port;

	std::atomic<bool> busy, ioTypesSet;
	
	ofJson infoJson;

	std::atomic<float> imageDownscaling;// default 0.25
	std::atomic<ofxRunwayState> state;
	
private:
	void makeRequest(const string& address, const ofJson& json);
};


