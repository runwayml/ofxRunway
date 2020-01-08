//
//  ofxRunwayData.hpp
//  ofxRunway_BM-densecap
//
//  Created by Roy Macdonald on 11/11/19.
//
//
#pragma once
#include "ofMain.h"
//-----------------------------------------------------------------------------------------------------------
enum ofxRunwayImageType{
	OFX_RUNWAY_JPG,
	OFX_RUNWAY_PNG
};
enum ofxRunwayPoseType{
	OFX_RUNWAY_POSE_NET,
	OFX_RUNWAY_PIF_PAF
};
enum ofxRunwayCaptionType{
	OFX_RUNWAY_COCO,
	OFX_RUNWAY_DENSE_CAP,
	OFX_RUNWAY_FACE_DETECTION
};
//-----------------------------------------------------------------------------------------------------------
// simple structure to hold the captions we get back from runway.
class ofxRunwayCaption{
public:
	ofxRunwayCaption(){}
	ofxRunwayCaption(const ofRectangle& _rect, const string& _label = ""): rect(_rect), label(_label){}
	ofRectangle rect;
	string label;
	// it is more practical to declare a draw function here rather than accessing the struct elements directly in the loop draw.
	void draw();
};

//-----------------------------------------------------------------------------------------------------------
/// Data structure that holds the info about the names of the body joints and how these are connected
class ofxRunwayPoseFeatures{
public:
	// this function has to be called within the runwayInfoEvent(ofJson& info) callback function
	bool setup(const ofJson& info);
	map<std::string, size_t> indexMap;
	vector< vector< size_t> > connections;
	size_t size() const {return indexMap.size();}
	bool isSetup() const {return bIsSetup;}
private:
	bool bIsSetup = false;
};
//-----------------------------------------------------------------------------------------------------------
/// Data structure that holds the info that runwayML extracts from human poses using models like PoseNet
class ofxRunwayPose{
public:
	vector<glm::vec2> joints;
	float score;
	ofRectangle boundingBox;
	void draw(const ofxRunwayPoseFeatures & features);
	void draw();
	
};
class ofxRunwayFaceLandmarks{
public:
	vector<glm::vec3> points;
	vector<std::string> labels;
	void buildMeshes();
	void draw(bool bDrawLabels = false);
protected:
	ofMesh lineMesh, pointMesh;
};

//-----------------------------------------------------------------------------------------------------------
class ofxRunwayData {
public:

	void setImage(const string& name,
				  const ofBaseHasPixels& pixels,
				  ofxRunwayImageType type, int resize_width = 0, int resize_height = 0);

	void setImage(const string& name,
				  const ofPixels& pixels,
				  ofxRunwayImageType type,
				  int resize_width = 0, int resize_height = 0);
	
	void setBoolean(const string& name, bool b);
	void setInt(const string& name, int i);
	void setFloat(const string& name, float f);
	void setString(const string& name, const string& s);
	void setFloats(const string& name, const vector<float>& f);
	void setStrings(const string& name, const vector<string>& s);
	void setFloatVectors(const string& name,const vector<vector<float> >& v);
	
	

	bool getImage(const string& name,ofBaseHasPixels& pixels);
	bool getImage(const string& name, ofPixels& pixels);
	bool getBoolean(const string& name, bool &b);
	bool getInt(const string& name, int& i);
	bool getFloat(const string& name, float& f);
	bool getString(const string& name, string& s);
	bool getFloats(const string& name, vector<float>& f);
	bool getStrings(const string& name, vector<string>& s);
	bool getFloatVectors(const string& name, vector<vector<float> >& v);
	
	bool getCaptions(vector<ofxRunwayCaption>& captions, float imgWidth, float imgHeight, ofxRunwayCaptionType capType);

	bool getPoses(vector<ofxRunwayPose>& poses, float imgWidth, float imgHeight, ofxRunwayPoseType poseType);

	bool getFaceLandmarks(vector<ofxRunwayFaceLandmarks>& landmarks, float imgWidth, float imgHeight);
	

	struct colorComp {
		bool operator()(const ofColor& a, const ofColor& b) const {
			return a.getHex() < b.getHex();
		}
	};
	
	typedef std::map<ofColor, string, colorComp> SegmentationMap;
	
	static bool getSegmentationMap(SegmentationMap & segMap, const ofJson& info);
	static string findSegmentationLabel(const SegmentationMap & segMap, const ofBaseHasPixels& pixels, size_t x, size_t y);
	static string findSegmentationLabel(const SegmentationMap & segMap, const ofPixels& pixels, size_t x, size_t y);
	ofJson data;
	
protected:
	bool checkSetData(const string& name, const string& callerName);
	template<typename T>
	bool getData(const string& name, const vector<string>& type_names, T& i, bool bIsVector = false);
	template<typename T>
	bool getDataArray(const string& name, const vector<string>& type_names, vector<T>& v);
};
