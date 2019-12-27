#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1280,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	//setup sphereVbo.
	// I order to draw a lot of spheres efficiently we are going to use a shader and instanced drawing.
	// Instanced drawing means that a simple mesh is passed to the shader and then it is the shader that draws this shape several times using some other data passed to the shader and the calculations made inside of it.
	//So, we use the ofSpherePrimitive to create a simple sphere of 1 pixel of radius.
	ofSpherePrimitive s(1, 20);
	sphereVbo = s.getMesh();
	
	//load the shader
	shader.load("shader");
	
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
	
	// setup the video grabber (camera)
	grabber.setup(640,480);
	
	
	// setup the gui
	gui.setup();
	gui.add(depthScale);
	gui.add(pointSize);
	gui.add(bDrawPoints);
	gui.add(pointStride);
	
}
//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	
	if(grabber.isFrameNew() && !runway.isBusy()){
		
		runway.send("image", grabber);
	}
	runway.get("depth_image", runwayResult);
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	// draw camera feed
	
	grabber.draw(0,0);
	
	// draw image received from Runway
	if(runwayResult.isAllocated()){
		runwayResult.draw(640,0);
	}
	ofEnableDepthTest();
	
	runway.drawStatus(660, 520, true);
	
	cam.begin();
	float stride = pointStride.get();
	glm::vec2 res;
	
	res.x = grabber.getWidth();
	res.y = grabber.getHeight();
	
	glm::vec2 img_size = res;
	
	res = res/stride;
	res.x = floor(res.x);
	res.y = floor(res.y);
	
	
	
	shader.begin();
	
	shader.setUniformTexture("video", grabber, 1);
	
	shader.setUniformTexture("depth", runwayResult, 2);
	
	shader.setUniform1f("depth_scale", depthScale.get());
	shader.setUniform1f("pointSize" , pointSize.get());
	shader.setUniform1f("stride", stride);
	shader.setUniform2f("res", res);
	shader.setUniform2f("img_size",img_size);
	
	size_t numInstances = res.x * res.y ;
	
	sphereVbo.drawInstanced(OF_MESH_FILL, numInstances);
	
	shader.end();
	cam.end();
	ofDisableDepthTest();
	
	
	
	
	
	gui.draw();
	
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == ' '){
		shader.load("shader");
	}
}

//--------------------------------------------------------------
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

