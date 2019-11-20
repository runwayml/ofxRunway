#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	promptForInput();
	
}
//--------------------------------------------------------------
void ofApp::update(){
	if(runway.get("text", runwayResult)){
		/// handy function for setting the clipboard (as if you'd copy the string)
		/// you can now paste this string on any other software
		ofSetClipboardString(runwayResult);
	}
}
//--------------------------------------------------------------
void ofApp::setPrompt(const string& _prompt){
	if(runway.isBusy()) return;
	if(_prompt.empty()) return;
	prompt = _prompt;
	
	ofxRunwayData data;
	
	data.setString("prompt", prompt);
	if(bUseRandomSeed) {
		seed = ofRandom(99999);
	}
	
	data.setInt("seed", seed);
	
	// query Runway
	runway.send(data);
}
//--------------------------------------------------------------
void ofApp::promptForInput(){
	
	if(runway.isBusy()) return;
	
	setPrompt(ofSystemTextBoxDialog("Put some text so runway can use as a starting point"));
	
}
//--------------------------------------------------------------
/// This is just a utility function that will format text so it wraps at a certain width.
string formatTextWidth(const string& text, float width){
	
	string formated;
	auto lines = ofSplitString(text, "\n", false, true);
	if(lines.size()){
		ofBitmapFont bf;
		float space_w = bf.getBoundingBox(" ", 0, 0).width;
		
		for(size_t l = 0; l < lines.size(); l++){
			auto words = ofSplitString(lines[l], " ");
			if(words.size()){
				formated += words[0];
				float accum = bf.getBoundingBox(words[0], 0, 0).width + space_w;
				float w;
				for(size_t i = 1; i < words.size(); i++){
					w = bf.getBoundingBox(words[i], 0, 0).width + space_w;
					if(accum + w - space_w > width){
						accum = w;
						formated += "\n" + words[i];
					}else{
						accum += w;
						formated += " " + words[i];
					}
					
				}
				if(l < lines.size()-1){
					formated += "\n";
				}
			}
		}
	}
	return formated;
}


//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(ofxRunway::gray);
	ofRectangle r = runway.drawStatus(20, 30, true);
	
	if(runway.isServerAvailable()){
		glm::vec3 margin(0, 30, 0);
		ofBitmapFont bf;
		
		string str = "Press SPACE to add a new prompt for Runway";
		
		auto pos = r.getBottomLeft() + margin;
		ofDrawBitmapString(str, pos);
		r = bf.getBoundingBox(str, pos.x, pos.y);
		
		// draw the prompt input by the user.
		
		str = "You sent to Runway the following prompt:\n";
		str+= "Using the " + (string)(bUseRandomSeed?"random":"non-random") + " seed: " + ofToString(seed);
		
		pos = r.getBottomLeft() + margin;
		ofDrawBitmapStringHighlight(str, pos, ofColor(40));
		r = bf.getBoundingBox(str, pos.x, pos.y);
		
		
		
		str = formatTextWidth(prompt, ofGetWidth() - 120 );
		pos = r.getBottomLeft() + margin;
		ofDrawBitmapStringHighlight(str, pos.x + 50, pos.y, ofxRunway::green1, ofxRunway::gray);
		r = bf.getBoundingBox(str, pos.x, pos.y);
		
		
		
		// draw the text returned by runway
		if(!runwayResult.empty()){
			
			str = "You got from Runway the following text:";
			pos = r.getBottomLeft() + margin;
			ofDrawBitmapStringHighlight(str, pos, ofColor(40));
			r = bf.getBoundingBox(str, pos.x, pos.y);

			
			str = formatTextWidth(runwayResult, ofGetWidth() - 120);
			pos = r.getBottomLeft() + margin;
			ofDrawBitmapStringHighlight(str, pos.x + 50, pos.y, ofxRunway::green2, ofColor::black);
			
		}
	}
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == ' '){
		promptForInput();
	}
}

// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
	ofLogNotice("ofApp::runwayInfoEvent") << info.dump(2);
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
//--------------------------------------------------------------
