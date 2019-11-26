#include "ofApp.h"
#ifndef USE_GRAB_CAM
//--------------------------
void ofApp::findCursor(const glm::vec2& mouse) {
	if(camViewport.inside(mouse)){
		GLint mouseViewportX = mouse.x - camViewport.x;
		GLint mouseViewportY = mouse.y - camViewport.y;
		
		const auto nearPlaneZ = (unsigned short) 32768;
		const auto farPlaneZ = (unsigned short) 65535;
		
		unsigned short z = farPlaneZ;
		
		int searchWidthPix = 6;
		//sampleRect will be in OpenGL's viewport coordinates
		ofRectangle sampleRect(-searchWidthPix / 2., -searchWidthPix / 2. , searchWidthPix, searchWidthPix);
		sampleRect.x += mouseViewportX;
		sampleRect.y += camViewport.height - 1 -mouseViewportY;
		
		sampleRect = sampleRect.getIntersection(camViewport);
		
		//this should always be true since findCursor is only called whilst cursor is inside viewport
		if (sampleRect.width > 0 && sampleRect.height > 0) {
			//check if we need to reallocate our local buffer for depth pixels
			if (sampleNeighbourhood.getWidth() != sampleRect.getWidth() || sampleNeighbourhood.getHeight() != sampleRect.getHeight()) {
				sampleNeighbourhood.allocate(sampleRect.getWidth(), sampleRect.getHeight(), OF_IMAGE_GRAYSCALE);
			}
			
			//sample depth pixels in the neighbourhood of the mouse
			glReadPixels(sampleRect.x, sampleRect.y, sampleRect.width, sampleRect.height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, sampleNeighbourhood.getData());
			
			//pick the closest pixel to use as a sample
			for (auto & pixel : sampleNeighbourhood) {
				//check that it's valid before using it
				if (pixel != nearPlaneZ && pixel != farPlaneZ) {
					z = MIN(pixel, z);
				}
			}
		}
		
		//check we're still looking at the near/far plane before updating the mouse distance
		glm::vec3 mouseProjected(mouse.x, mouse.y,0);
		if (z != nearPlaneZ && z != farPlaneZ) {
			mouseProjected.z = ((float)z / (float)USHRT_MAX) * 2.0f - 1.0f;
		}
		auto tpos = cam.getTarget().getPosition();
		cam.setTarget (cam.screenToWorld(mouseProjected,camViewport));
		cam.lookAt(tpos);
	}
}
#endif
void ofApp::loadDepthImages(const string& dirpath){
	ofDirectory dir;
	dir.allowExt("png");
	dir.listDir(dirpath);
	depthImgs.resize(dir.size());
	dir.sort();
	for(size_t i = 0; i < dir.size(); i++){
		ofLoadImage(depthImgs[i], dir.getPath(i));
		cout << "i " << i << dir.getPath(i) << endl;
	}
	
}
//--------------------------------------------------------------
void ofApp::loadDialog(){
	auto playerDialog = ofSystemLoadDialog("select video file");
	if(playerDialog.bSuccess){
		if(player.load(playerDialog.getPath())){
			cout << "Player path : " << playerDialog.getPath() << endl;
			auto depthDialog = ofSystemLoadDialog("select folder with depth image sequence", true);
			if(depthDialog.bSuccess){
				cout << "depthDialog : " << depthDialog.getPath()<<endl;
				loadDepthImages(depthDialog.getPath());
			}
		}
	}
}
//--------------------------------------------------------------
void ofApp::load(const string& video, const string& depthDir){
	if(player.load(video)){
		loadDepthImages(depthDir);
	}
}
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
	//uncomment the following line to hard code the path to the movie file and depth frames. Make sure you put the correct paths!
	// Also comment or remove 'loadDialog();'
//	load("path-to-your-movie-file.mp4", "path/to/the/depth/images/folder/");
	loadDialog();
	
	#ifdef USE_GRAB_CAM
	cam.setFixUpDirectionEnabled(true);
	cam.setFarClip(10000);
	#endif
	
	// setup the gui
	gui.setup();
	gui.add(depthScale);
	gui.add(pointSize);
	gui.add(bDrawPoints);
	gui.add(pointStride);
	
}
//--------------------------------------------------------------
void ofApp::update(){
	player.update();
	
	if(player.isFrameNew()){
		auto f = player.getCurrentFrame();
		if(f < depthImgs.size()){
			depthTex.loadData(depthImgs[f]);
		}
	}
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	
	ofRectangle s(0,0,ofGetWidth()/2, ofGetHeight());
	camViewport.set(0,0,ofGetWidth(), ofGetHeight());
	
	if(	bDrawVideo){
		ofRectangle pr(0,0, player.getWidth(), player.getHeight());
		pr.scaleTo(s);
		pr.y = 0;
		player.draw(pr);
		
		s.x += s.width;
		camViewport.y = pr.height;
	}
	if(	bDrawDepth){
		ofRectangle dr(0,0, depthTex.getWidth(), depthTex.getHeight());
		dr.scaleTo(s);
		dr.y = 0;
		camViewport.y = dr.height;
		
		depthTex.draw(dr);
		
	}
	
	if(bDrawInfo){
		stringstream ss;
		ss << "Press [key]:" << endl;
		ss << "      [ g ]:  toggle Draw GUI" << endl;
		ss << "      [ v ]:  toggle Draw Video" << endl;
		ss << "      [ d ]:  toggle Draw Depth" << endl;
		ss << "      [ i ]:  toggle Draw this info" << endl;
		ss << endl << "Framerate: " << ofGetFrameRate() <<endl;
		ss << "player current frame: " << player.getCurrentFrame() <<endl;
		
		
		
		ofDrawBitmapString(ss.str(), 20, camViewport.y + 20);
		ofBitmapFont bf;
		
		camViewport.y = bf.getBoundingBox(ss.str(), 20, camViewport.y + 20).getMaxY() + 20;
		
	}
	
	camViewport.height -= camViewport.y;
	
	
	ofEnableDepthTest();
	
	cam.begin(camViewport);
	float stride = pointStride.get();
	glm::vec2 res;
	
	res.x = player.getWidth();
	res.y = player.getHeight();
	
	glm::vec2 img_size = res;
	
	res = res/stride;
	res.x = floor(res.x);
	res.y = floor(res.y);
	
	
	shader.begin();
	
	shader.setUniformTexture("video", player, 1);
	
	shader.setUniformTexture("depth", depthTex, 2);
	
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
	
	
	
	if(bDrawGui) gui.draw();
	
	
	
	
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'g'){
		bDrawGui^= true;
	}else if(key == 'v'){
		bDrawVideo^= true;
	}else if(key == 'd'){
		bDrawDepth^= true;
	}else if(key == 'i'){
		bDrawInfo^= true;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
#ifndef USE_GRAB_CAM
	findCursor({x, y});
#endif
}
