#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
  // match sketch size to default model camera setup
  size(1200,400);
  // setup Runway
  ofLog::setChannel(std::make_shared<ofxIO::ThreadsafeConsoleLoggerChannel>());
runway.setup("http://localhost:8000");
runway.start();
  // update manually
  //runway.setAutoUpdate(false);
}

//--------------------------------------------------------------
void ofApp::draw(){
//  background(0);
  // draw content image
  if(contentImage.isAllocated()){contentImage.draw(0,0);}
  // draw image received from Runway
  if(runwayResult.isAllocated()){runwayResult.draw(600,0);}
  // display status
  ofDrawBitmapString(status, 5, 5);
}
//--------------------------------------------------------------
void keyPressed(){
  if(key == 'c'){
    contentImageSelected(ofSystemLoadDialog("Select a content image to process:"));

  }
}
//--------------------------------------------------------------
void ofApp::loadContentImage(){
  auto resp = ofSystemLoadDialog();
if(!resp.bSuccess) {
    ofLogVerbose("ofApp") << "Window was closed or the user hit cancel." ;
  } else {
    println("selected " + resp.getPath());
    contentImage.load(resp.getPath());
    // resize image (adjust as needed)
    contentImage.resize(600,400);
    // send it to Runway
    runway.query(contentImage,ModelUtils.IMAGE_FORMAT_JPG,"contentImage");
  }
}

// this is called when new Runway data is available
//-------------------------------------------------------------- 
void ofApp::runwayDataEvent(ofJson& runwayData){
  // point the sketch data to the Runway incoming data 
  String base64ImageString = runwayData.getString("stylizedImage");
  // try to decode the image from
  try{
    runwayResult = ModelUtils.fromBase64(base64ImageString);
  }catch(Exception e){
    e.printStackTrace();
  }
  status = "received runway result";
}

// this is called each time Processing connects to Runway
// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
  println(info);
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
  println(message);
}
