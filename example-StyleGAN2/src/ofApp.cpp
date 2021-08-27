#include "ofApp.h"

/*

    an example of GAN inference (generating an image from a GAN)
    this is setup for a network that wants an input vector z of 512 values
    and a truncation value, but should work with others if you modify it

    try using StyleGAN2 with RunwayML to generate some faces
    this example assumes Local GPU

*/

//--------------------------------------------------------------

void ofApp::setup()
{
    // uncomment the following line if you want a verbose log (which means a lot of info will be printed)
    // ofSetLogLevel(OF_LOG_VERBOSE);

    ofSetWindowShape(512, 512);

    // setup Runway
    runway.setup(this, "http://localhost:8000");
    runway.start();

    // infer a new image to begin
    generate_image(generate_random_z(), truncation);
}
//--------------------------------------------------------------
void ofApp::update()
{
    // structuring it this way allows you use tryReceive
    ofxRunwayData dataToReceive;

    // infer/generate a new image if we have sent a request to Runway
    // using tryReceive we can make sure there is data available before grabbing it
    while (runway.tryReceive(dataToReceive))
    {
       dataToReceive.getImage("image", currentImg);
       currentImg.update();
    }
}
//--------------------------------------------------------------
void ofApp::draw()
{

    // draw image received from Runway
    if (currentImg.isAllocated())
    {
        currentImg.draw(0, 0);
    }

    // draw Runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
    ofRectangle r = runway.drawStatus(620, 440, true);
    ofDrawBitmapString("Press ' ' to send to Runway", r.getBottomLeft() + glm::vec3(0, 20, 0));


    // save frame (useful for latent space walks)
    // string fn = ofToString(ofGetFrameNum(),4,'0')+".png";
    // cout << fn << endl;
    // ofSaveScreen(fn);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    // on spacebar, infer a new image
    if (key == ' ')
    {
        generate_image(generate_random_z(), truncation);
    }
}

//--------------------------------------------------------------
vector<float> ofApp::generate_random_z()
{
    // this is a helpful method to generate a random set of input values

    vector<float> z; // create an empty vector of floats
    for (int i = 0; i < 512; i++)
    {
        z.push_back(ofRandom(-min_max_vals, min_max_vals));
    }

    return z;
}

//--------------------------------------------------------------
void ofApp::generate_image(vector<float> z, float truncation)
{
    // skip if content image isn't loaded yet

    if (runway.isBusy())
        return;

    // create a data object to send
    ofxRunwayData data;
    data.setFloats("z", z); // add our vector of random floats
    data.setFloat("truncation", truncation);    // also need to set a truncation

    // basically, the higher the truncation value the weirder the results

    runway.send(data);
}

// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson &info)
{
    ofLogNotice("ofApp::runwayInfoEvent") << info.dump(2);
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string &message)
{
    ofLogNotice("ofApp::runwayErrorEvent") << message;
}
