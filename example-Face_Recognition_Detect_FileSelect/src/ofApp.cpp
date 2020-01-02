#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup() {
  size(600, 400);
  frameRate(3);
  fill(255);
  stroke(255);
  // setup Runway
  ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
runway.setup("http://localhost:8000");
runway.start();
  // disable automatic polling: request data manually when a new frame is ready
  //runway.setAutoUpdate(false);
}

//--------------------------------------------------------------
void ofApp::draw() {
//  background(0);
  
  // Display label image if loaded
  if(inputImage.isAllocated()){inputImage.draw(0,0);}
  
  // Display instructions
  ofDrawBitmapString("press 's' to select an image\npress SPACE to query Runway", 5, 5);
  
  // Display captions
  drawCaptions();
}
//--------------------------------------------------------------
void keyPressed(){
  if(key == 's'){
    inputImageSelected(ofSystemLoadDialog("Select an input image to process:"));

  }
  if(key == ' '){
    if(inputImage != null){
      runway.query(inputImage);
    }  
  }
}
//--------------------------------------------------------------
void ofApp::loadInputImage(){
  auto resp = ofSystemLoadDialog();
if(!resp.bSuccess) {
    ofLogVerbose("ofApp") << "Window was closed or the user hit cancel." ;
  } else {
    println("selected " + resp.getPath());
    inputImage.load(resp.getPath());
    // resize image (adjust as needed)
    inputImage.resize(600,400);
  }
}

// A function to display the captions
//--------------------------------------------------------------
void ofApp::drawCaptions() {
  // if no data is loaded yet, exit
  if(data == null){
    return;
  }
  
  // access boxes and labels JSON arrays within the result
  JSONArray results = data.getJSONArray("results");
  // for each array element
  for(int i = 0 ; i < results.size(); i++){
    JSONObject result = results.getJSONObject(i);
    
    String className = result.getString("class");
    JSONArray box = result.getJSONArray("bbox");
    // extract values from the float array
    float x = box.getFloat(0);
    float y = box.getFloat(1);
    float w = box.getFloat(2);
    float h = box.getFloat(3);
    // display bounding boxes
    noFill();
    rect(x,y,w,h);
    fill(255);
    text(className,x,y);
  }
}



// this is called when new Runway data is available
//-------------------------------------------------------------- 
void ofApp::runwayDataEvent(ofJson& runwayData){
  // point the sketch data to the Runway incoming data 
  data = runwayData;
}
