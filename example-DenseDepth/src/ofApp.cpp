#include "ofApp.h"
//--------------------------------------------------------------

void ofApp::setup(){
	// make the app window large enough to fit all three images
	ofSetWindowShape(1280,800);
	
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	//ofSetLogLevel(OF_LOG_VERBOSE);

	
	ofSpherePrimitive s(1, 20);
	
	sphereVbo = s.getMesh();
	
	auto verts = s.getMesh().getVertices();
	
	for(auto&v : verts){
		cout << v.x * v.x + v.y * v.y + v.z * v.z<< endl;
	}
	
	shader.load("shader");
#ifdef USE_RUNWAY
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
#endif
	
	// setup camera
	#ifdef USE_CAM_GRABBER
	grabber.setup(640,480);
	#endif
	
//	plane.set(640,480,640,480);
//	plane.mapTexCoordsFromTexture(grabber.getTexture());
//	plane.getMesh().setMode(OF_PRIMITIVE_POINTS);
	gui.setup();
	gui.add(depthScale);
	gui.add(pointSize);
	gui.add(bDrawPoints);
	gui.add(pointStride);
//	cam.setVFlip(true);
}
//--------------------------------------------------------------
void ofApp::update(){
	#ifdef USE_CAM_GRABBER
	grabber.update();
	#endif
#ifdef USE_RUNWAY
	if(grabber.isFrameNew()){
		if(runway.isBusy()) return;
		
		
		runway.send("image", grabber);
	}
	runway.get("depth_image", runwayResult);
#endif
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	// draw camera feed
	#ifdef USE_CAM_GRABBER
	grabber.draw(0,0);
	#endif
	// draw image received from Runway
	if(runwayResult.isAllocated()){
		runwayResult.draw(640,0);
	}
		ofEnableDepthTest();
		cam.begin();
//		if(bDrawPoints){
//			gl(pointSize.get());
//		}
		float stride = pointStride.get();
	glm::vec2 res;
#ifdef USE_CAM_GRABBER
	res.x = grabber.getWidth();
	res.y = grabber.getHeight();
#else
	res.x = 640.;
	res.y = 480.;
#endif
	glm::vec2 img_size = res;
	
	res = res/stride;
	res.x = floor(res.x);
	res.y = floor(res.y);
	
	
		shader.begin();
#ifdef USE_CAM_GRABBER
	shader.setUniformTexture("video", grabber, 1);
#endif
#ifdef USE_RUNWAY
		shader.setUniformTexture("depth", runwayResult, 2);
#endif
		shader.setUniform1f("depth_scale", depthScale.get());
		shader.setUniform1f("pointSize" , pointSize.get());
		shader.setUniform1f("stride", stride);
		shader.setUniform2f("res", res);
		shader.setUniform2f("img_size",img_size);
		
//		shader.setUniform1f("pointStride", (bDrawPoints?pointStride.get():1));

		
//		shader.setUniform1f("x_res", x_res);
		
		size_t numInstances = res.x * res.y ;
		
		sphereVbo.drawInstanced(OF_MESH_FILL, numInstances);
		
		shader.end();
		cam.end();
		ofDisableDepthTest();
//	}
#ifdef USE_RUNWAY
	ofRectangle r = runway.drawStatus(660, 520, true);
#endif
	
	stringstream ss;
	ss << "res " << res << endl;
	
	ofDrawBitmapString(ss.str(), 200, 20);
	
	gui.draw();
	
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == ' '){
		shader.load("shader");
	}
}
#ifdef USE_RUNWAY
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
#endif
