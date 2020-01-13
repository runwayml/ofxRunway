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
	std::string label;
	// it is more practical to declare a draw function here rather than accessing the struct elements directly in the loop draw.
	void draw();
};

//-----------------------------------------------------------------------------------------------------------
/// Data structure that holds the info about the names of the body joints and how these are connected
class ofxRunwayPoseFeatures{
public:
	// this function has to be called within the runwayInfoEvent(ofJson& info) callback function
	bool setup(const ofJson& info);
	std::map<std::string, size_t> indexMap;
	std::vector< std::vector< size_t> > connections;
	size_t size() const {return indexMap.size();}
	bool isSetup() const {return bIsSetup;}
private:
	bool bIsSetup = false;
};
//-----------------------------------------------------------------------------------------------------------
/// Data structure that holds the info that runwayML extracts from human poses using models like PoseNet
class ofxRunwayPose{
public:
	std::vector<glm::vec2> joints;
	float score;
	ofRectangle boundingBox;
	void draw(const ofxRunwayPoseFeatures & features);
	void draw();
	
};
class ofxRunwayFaceLandmarks{
public:
	std::vector<glm::vec3> points;
	std::vector<std::string> labels;
	void buildMeshes();
	void draw(bool bDrawLabels = false);
protected:
	ofMesh lineMesh, pointMesh;
};

//-----------------------------------------------------------------------------------------------------------
class ofxRunwayData {
public:

	void setImage(const std::string& name,
				  const ofBaseHasPixels& pixels,
				  ofxRunwayImageType type, int resize_width = 0, int resize_height = 0);

	void setImage(const std::string& name,
				  const ofPixels& pixels,
				  ofxRunwayImageType type,
				  int resize_width = 0, int resize_height = 0);
	
	void setBoolean(const std::string& name, bool b);
	void setInt(const std::string& name, int i);
	void setFloat(const std::string& name, float f);
	void setString(const std::string& name, const std::string& s);
	void setFloats(const std::string& name, const std::vector<float>& f);
	void setStrings(const std::string& name, const std::vector<std::string>& s);
	void setFloatVectors(const std::string& name,const std::vector<std::vector<float> >& v);
	
	

	bool getImage(const std::string& name,ofBaseHasPixels& pixels);
	bool getImage(const std::string& name, ofPixels& pixels);
	bool getBoolean(const std::string& name, bool &b);
	bool getInt(const std::string& name, int& i);
	bool getFloat(const std::string& name, float& f);
	bool getString(const std::string& name, std::string& s);
	bool getFloats(const std::string& name, std::vector<float>& f);
	bool getStrings(const std::string& name, std::vector<string>& s);
	bool getFloatVectors(const std::string& name, std::vector<vector<float> >& v);
	
	bool getCaptions(std::vector<ofxRunwayCaption>& captions, float imgWidth, float imgHeight, ofxRunwayCaptionType capType);

	bool getPoses(std::vector<ofxRunwayPose>& poses, float imgWidth, float imgHeight, ofxRunwayPoseType poseType);

	bool getFaceLandmarks(std::vector<ofxRunwayFaceLandmarks>& landmarks, float imgWidth, float imgHeight);
	

	struct colorComp {
		bool operator()(const ofColor& a, const ofColor& b) const {
			return a.getHex() < b.getHex();
		}
	};
	
	typedef std::map<ofColor, std::string, colorComp> SegmentationMap;
	
	static bool getSegmentationMap(SegmentationMap & segMap, const ofJson& info);
	static std::string findSegmentationLabel(const SegmentationMap & segMap, const ofBaseHasPixels& pixels, size_t x, size_t y);
	static std::string findSegmentationLabel(const SegmentationMap & segMap, const ofPixels& pixels, size_t x, size_t y);
	ofJson data;
	
protected:
	bool checkSetData(const std::string& name, const std::string& callerName);
	template<typename T>
	bool getData(const std::string& name, const std::vector<std::string>& type_names, T& i, bool bIsVector = false);
	template<typename T>
	bool getDataArray(const std::string& name, const std::vector<std::string>& type_names, std::vector<T>& v);
};
