//
//  ofxRunwayData.cpp
//  ofxRunway_BM-densecap
//
//  Created by Roy Macdonald on 11/11/19.
//
//

#include "ofxRunwayData.h"
#include "ofxIO.h"
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::checkSetData(const string& name, const string& callerName){
	if(data.contains(name)){
		ofLogWarning("ofxRunwayData::"+callerName) << "there is already data with name \"" << name << "\". It will be overwritten";
		return false;
	}
	return true;
}


//------------------------------------------------------------------------------------------------
void ofxRunwayData::setImage(const string& name,
							 const ofBaseHasPixels& pixels,
							 ofxRunwayImageType type, int width, int height){
	this->setImage(name, pixels.getPixels(), type, width, height);
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setImage(const string& name,
							 const ofPixels& pixels,
							 ofxRunwayImageType type,
							 int width, int height){
	
	checkSetData(name, "setImage");
	
	ofPixels pixelsToReceive = pixels;
	
	//make sure we dont resize to zero or that we upscale (which makes no sense)
	if(width != 0 && height != 0 &&
	   width < pixelsToReceive.getWidth() &&
	   height < pixelsToReceive.getHeight()){
		
		ofRectangle original(0,0, pixelsToReceive.getWidth(),
							 pixelsToReceive.getHeight());
		ofRectangle resized(0,0,width, height);
		// with this we make sure that we scale proportionaly
		original.scaleTo(resized);
		
		pixelsToReceive.resize(original.width, original.height);
	}
	// Save the incoming pixels to a buffer using JPG compression.
	ofBuffer compressedPixels;
	ofSaveImage(pixelsToReceive, compressedPixels, (type == OFX_RUNWAY_JPG)?OF_IMAGE_FORMAT_JPEG:OF_IMAGE_FORMAT_PNG);
	
	// Encode the compressed pixels in base64.
	ofxIO::ByteBuffer base64CompressedPixelsIn;
	ofxIO::Base64Encoding base64Encoder;
	base64Encoder.encode(ofxIO::ByteBuffer(compressedPixels.getData(), compressedPixels.size()), base64CompressedPixelsIn);
	
	string header;
	if(type == OFX_RUNWAY_JPG){
		header = "data:image/jpeg;base64,";
	}else if(type == OFX_RUNWAY_PNG){
		header  = "data:image/png;base64,";
	}else{
		ofLogError("ofxRunwayData::setImage") << "invalid image type. Must be OFX_RUNWAY_JPG or OFX_RUNWAY_PNG";
	}
	
	data[name] = header + base64CompressedPixelsIn.toString()+"==";
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setBoolean(const string& name, bool b){
	checkSetData(name, "setBoolean");
	data[name] = b;
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setInt(const string& name, int i){
	checkSetData(name, "setInt");
	data[name] = i;
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setFloat(const string& name, float f){
	checkSetData(name, "setFloat");
	data[name] = f;
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setString(const string& name, const string& s){
	checkSetData(name, "setString");
	data[name] = s;
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setFloats(const string& name, const vector<float>& f){
	checkSetData(name, "setFloats");
	data[name] = f;
}
//------------------------------------------------------------------------------------------------
void ofxRunwayData::setStrings(const string& name, const vector<string>& s){
	checkSetData(name, "setStrings");
	data[name] = s;
}


//------------------------------------------------------------------------------------------------
void ofxRunwayData::setFloatVectors(const string& name,const vector<vector<float> >& v){
	checkSetData(name, "setFloatVectors");
	data[name] = v;
}

//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getImage(const string& name,
							 ofBaseHasPixels& pixels
							 ){
	return getImage(name, pixels.getPixels());
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getImage(const string& name,
							 ofPixels& pixels
							 ){
	if(!data.contains(name)){
		ofLogError("ofxRunwayData::getData") << "there is no image data with name \"" << name << "\".";
		return false;
	}
	
	string type = data[name].type_name();
	if (type == "image" || type == "string"){
		string imageB64 = data[name].dump();
		imageB64 = imageB64.substr(1, imageB64.size()-2);//+"==";;
		imageB64 = imageB64.substr(imageB64.find(",") + 1);
		
		ofBuffer decodedBuffer;
		decodedBuffer.set(ofxIO::Base64Encoding::decode(imageB64));
		
		ofLoadImage(pixels, decodedBuffer);
		
		return true;
	}else{
		cout << "ofxRunwayData::getImage " << type <<endl;
	}
	return false;
}
//------------------------------------------------------------------------------------------------
template<typename T>
bool ofxRunwayData::getData(const string& name, const vector<string>& type_names, T& i, bool bIsVector){
	if(!data.contains(name)){
		ofLogError("ofxRunwayData::getData") << "there is no data with name \"" << name << "\".";
		return false;
	}
	string type = data[name].type_name();
	if(bIsVector && type == "array"){
		type = data[name][0].type_name();
	}
	
	for(auto& tn: type_names){
		if (type == tn){
			if(bIsVector && data[name].size() == 0){
				ofLogVerbose("ofxRunwayData::getData") << "data found but is empty";
				return false;
			}
			T t = data[name];
			
			i = t;
			return true;
		}
	}
	stringstream ss;
	for(auto& tn: type_names){
		ss << "There is no data with types \"" << tn << ", ";
	}
	ofLogError("ofxRunwayData::getData") << ss.str() << "\" and name \"" << name << "\"";
//	cout << data.dump(2) << endl;
	return false;
}
//------------------------------------------------------------------------------------------------
template<typename T>
bool ofxRunwayData::getDataArray(const string& name, const vector<string>& type_names, vector<T>& v){
	if(!data.contains(name)){
		ofLogError("ofxRunwayData::getDataArray") << "there is no data with name \"" << name << "\".";
		return false;
	}
	string type = data[name].type_name();
	if(type != "array"){
		ofLogError("ofxRunwayData::getDataArray") << "data with name \"" << name << "\" is not an array.";
		return false;
	}
	return getData(name, type_names, v, true);
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getBoolean(const string& name, bool& b){
	return getData(name, {"boolean", "bool"}, b);
}

//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getInt(const string& name, int& i){
	return getData(name, {"int"}, i);
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getFloat(const string& name, float& f){
	return getData(name, {"float"}, f);
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getString(const string& name, string& s){
	return getData(name, {"string","text"}, s);
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getFloats(const string& name, vector<float>& f){
	return getDataArray(name, {"float"}, f);
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getStrings(const string& name, vector<string>& s){
	return getDataArray(name, {"string","text"}, s);
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getFloatVectors(const string& name, vector<vector<float> >& v){
	return getDataArray(name, {"array"}, v);
}

//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getCaptions(vector<ofxRunwayCaption>& captions, float imgWidth, float imgHeight, ofxRunwayCaptionType capType){
	ofJson boxes;
	ofJson labels;
	ofJson scores;
	if(capType == OFX_RUNWAY_COCO){
		if(data.count("boxes")){
			boxes = data["boxes"];// used with COCO
			labels = data["labels"];
		}
	}
	else if(capType == OFX_RUNWAY_DENSE_CAP){
		if(data.count("bboxes")){
			boxes = data["bboxes"];//used with DenseCap
			labels = data["classes"];
			scores = data["scores"];
		}
	}
	else if(capType == OFX_RUNWAY_FACE_DETECTION){
		vector<vector<float> > vv;
		captions.clear();
		if(getFloatVectors("results",vv)){
			for(auto& v: vv){
				if(v.size() != 4 ){
					ofLogWarning("ofxRunwayData::getFaceRects") << "results is not four: not adding to rectangles";
					continue;
				}
			
				captions.push_back(ofxRunwayCaption(ofRectangle({v[0] * imgWidth, v[1] * imgHeight}, {v[2] * imgWidth, v[3] * imgHeight})));
								   
			}
			return true;
		}
	}
	if(capType == OFX_RUNWAY_DENSE_CAP || capType == OFX_RUNWAY_COCO){
		// as long the array sizes match
		if(boxes.size() == labels.size() && boxes.size() > 0){
			// for each array element
			captions.resize(boxes.size());
			for(int i = 0 ; i < boxes.size(); i++){
				
				captions[i].label = labels[i];
				if(capType == OFX_RUNWAY_DENSE_CAP){
					captions[i].label += "\nscore: " +ofToString((float)scores[i]);
				}
				// extract values from the float array
				captions[i].rect.x = (float)boxes[i][0] * imgWidth;
				captions[i].rect.y = (float)boxes[i][1] * imgHeight;
				captions[i].rect.width = ((float)boxes[i][2] * imgWidth) - captions[i].rect.x;
				captions[i].rect.height = ((float)boxes[i][3] * imgHeight) - captions[i].rect.y;
				
			}
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getPoses(vector<ofxRunwayPose>& poses, float imgWidth, float imgHeight, ofxRunwayPoseType poseType){
	
	if(poseType == OFX_RUNWAY_POSE_NET){
		ofJson jpos;
		ofJson scores;
	
		if(data.count("poses")){
			jpos = data["poses"];
			scores = data["scores"];
		}
		
		//	std::cout << "imgWidth: " << imgWidth << " imgHeight: " << imgHeight << std::endl;
		// as long the array sizes match
		if(jpos.size() == scores.size() && jpos.size() > 0){
			// for each array element
			poses.resize(jpos.size());
			for(size_t i = 0 ; i < poses.size(); i++){
				poses[i].joints.resize(jpos[i].size());
				for(size_t j = 0; j < poses[i].joints.size(); j++){
					poses[i].joints[j] = {(float)jpos[i][j][0] * imgWidth, (float)jpos[i][j][1] * imgHeight};
				}
				poses[i].score = scores[i];
			}
			return true;
		}
	}else if(poseType == OFX_RUNWAY_PIF_PAF){

		if(data.count("keypoints")){
			
			ofJson keypoints;
			keypoints = ofJson::parse((string)data["keypoints"]);
			  
				poses.resize(keypoints.size());
		
				for(size_t i = 0; i < keypoints.size(); i++ ){
					ofJson pose = keypoints[i];
					ofJson points = pose["keypoints"];
					ofJson bbox = pose["bbox"];
		
					poses[i].boundingBox.set((float)bbox[0],(float)bbox[1], (float)bbox[2], (float)bbox[3]);
					for(size_t j = 0; j < points.size(); j+=3){
						poses[i].joints.push_back({(float)points[i], (float)points[i+1]});
					}
				}
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getFaceLandmarks(vector<ofxRunwayFaceLandmarks>& landmarks, float imgWidth, float imgHeight){
	ofJson points;
	ofJson labels;
	
	if(data.count("points")){
		points = data["points"];
		labels = data["labels"];
		if(points.size() != labels.size()){
			ofLogVerbose("ofxRunwayData::getFaceLandmarks") << "different number of points and labels";
			return false;
		}
		landmarks.resize(1);
		landmarks[0].points.resize(points.size());
		landmarks[0].labels.resize(points.size());
		for(size_t i = 0; i < points.size(); i++){
			landmarks[0].points[i].x = (float)points[i][0] * imgWidth;
			landmarks[0].points[i].y = (float)points[i][1] * imgHeight;
			landmarks[0].points[i].z = 0;
			landmarks[0].labels[i] = (string)labels[i];
		}
		landmarks[0].buildMeshes();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------
bool ofxRunwayData::getSegmentationMap(SegmentationMap & segMap, const ofJson& info){
	
	segMap.clear();
	if(info.count("outputs")){
		for(auto&o: info["outputs"]){
			if(o["type"] == "segmentation"){
				auto& l = o["labelToColor"];
				for (auto it = l.begin(); it != l.end(); ++it){
					auto &col = it.value();
					ofColor c ((int)col[0], (int)col[1],(int)col[2]);
					string key = it.key();
					segMap[c] = key;
				}
				return true;
				break;
			}
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------
string ofxRunwayData::findSegmentationLabel(const SegmentationMap & segMap, const ofBaseHasPixels& pixels, size_t x, size_t y){
	return findSegmentationLabel(segMap, pixels.getPixels(), x, y);
}
//------------------------------------------------------------------------------------------------
string ofxRunwayData::findSegmentationLabel(const SegmentationMap & segMap, const ofPixels& pixels, size_t x, size_t y){
	if(x < pixels.getWidth() && y < pixels.getHeight()){
		auto c = pixels.getColor(x, y);
		if(segMap.count(c)){
			return segMap.at(c);
		}else{
			ofLogVerbose("ofxRunwayData::findSegmentationLabel") << " color " << c << " not found.";
			return "";
		}
	}else{
		ofLogVerbose("ofxRunwayData::findSegmentationLabel") << "coords out of bounds ";
	}
	return "";
}
//------------------------------------------------------------------------------------------------
void ofxRunwayCaption::draw(){
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(2);
	ofSetColor(0);
	ofDrawRectangle(rect);
	ofDrawBitmapStringHighlight(label, rect.getTopLeft() + glm::vec3(3,20,0));
	ofPopStyle();
}
//------------------------------------------------------------------------------------------------
bool ofxRunwayPoseFeatures::setup(const ofJson& info){
	bool bSuccess = false;
	//	if(info.count("outputs") == 0){
	//		ofLogWarning("ofxRunwayPoseFeatures::setup", "Failed. JSON might be formated incorrectly. no \"output\" element");
	//		return  false;
	//	}
	if(info.size() == 0) return false;
	if(info[0].count("itemType") == 0){
		ofLogWarning("ofxRunwayPoseFeatures::setup", "Failed. JSON might be formated incorrectly. no \"output/itemType\" element");
		return  false;
	}
	
	ofJson items = info[0]["itemType"];
	if(items.count("length") == 0){
		ofLogWarning("ofxRunwayPoseFeatures::setup", "Failed. JSON might be formated incorrectly. no \"output/itemType/length\" element");
		return  false;
	}
	
	size_t numItems = items["length"];
	
	if(items.count("labels") == 0){
		ofLogWarning("ofxRunwayPoseFeatures::setup", "Failed. JSON might be formated incorrectly. no \"output/itemType/labels\" element");
		return  false;
	}
	
	ofJson labels = items["labels"];
	for(size_t i = 0; i < numItems; i++){
		indexMap[(string)labels.at(i)] = i;
	}
	
	
	if(items.count("connections") == 0){
		ofLogWarning("ofxRunwayPoseFeatures::setup", "Failed. JSON might be formated incorrectly. no \"output/itemType/connections\" element");
		return  false;
	}
	ofJson conn = items["connections"];
	
	
	//	cout << "ofxRunwayPoseFeatures::setup numItems " << numItems << endl;
	for(auto& c: conn){
		string j0 = c[0];
		string j1 = c[1];
		
		if(indexMap.count(j0) && indexMap.count(j1)){
			connections.push_back({ indexMap[j0], indexMap[j1] });
			bSuccess = true;
		}else{
			cout << "indexMap does not have element " << j0 << " or " << j1 << endl;
			bSuccess = false;
		}
	}
	if(!bSuccess){
		
		ofLogWarning("ofxRunwayPoseFeatures::setup", "Failed. JSON might be formated incorrectly");
	}
	bIsSetup = bSuccess;
	return bSuccess;
}
//------------------------------------------------------------------------------------------------
void ofxRunwayPose::draw(){
	ofPushStyle();

	ofSetColor(ofColor::yellow);
	for(auto&j: joints){
		ofDrawCircle(j, 5);
	}
	
	ofPopStyle();
}


//------------------------------------------------------------------------------------------------
void ofxRunwayPose::draw(const ofxRunwayPoseFeatures & features){
	ofPushStyle();
	
	if(features.isSetup() && features.size() == joints.size()){
		ofSetLineWidth(3);
		ofSetColor(ofColor::magenta);
		for(auto& c: features.connections){
			ofDrawLine(joints[c[0]], joints[c[1]]);
		}
	}
	ofSetColor(ofColor::yellow);
	for(auto&j: joints){
		ofDrawCircle(j, 5);
	}
	
	ofPopStyle();
}
//------------------------------------------------------------------------------------------------
void ofxRunwayFaceLandmarks::buildMeshes(){
	lineMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	pointMesh.setMode(OF_PRIMITIVE_POINTS);
	
	lineMesh.clear();
	pointMesh.clear();
	
	lineMesh.addVertices(points);
	pointMesh.addVertices(points);
	
}
//------------------------------------------------------------------------------------------------
void ofxRunwayFaceLandmarks::draw(bool bDrawLabels){
	ofPushStyle();
	ofSetColor(ofColor::red);
	ofSetLineWidth(3);
	lineMesh.draw();
	glPointSize(5);
	pointMesh.draw();
	ofPopStyle();
	if(bDrawLabels){
		auto n = std::min(points.size(), labels.size());
		glm::vec3 offset ( 5, 7, 0);
		for(size_t i = 0; i < n; i ++){
			ofDrawBitmapString(labels[i], points[i]+ offset);
		}
	}
	
}





