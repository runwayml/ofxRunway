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
	
	pixelsToReceive.resize(width, height);
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



