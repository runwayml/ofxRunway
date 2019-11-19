<p align="center">
  <img src="./assets/cover_runway_openframeworks_github.jpg">
</p>

#  ofxRunway
### RunwayML + openFrameworks

[openFrameworks](http://openframeworks.cc/) addon that allows you to easily integrate  [RunwayML](https://runwayml.com/) into your openFrameworks App. 
It contains examples on most of the currently available models.

## Install
Download and copy or `git clone` this addon into your openFrameworks addons folder.

Generate or update the example projects by dragging their folder onto the projectGenerator and hitting "update" or "generate".

## Dependencies

You'll need to install the following addons for this addon to work.

* [ofxHTTP](https://github.com/bakercp/ofxHTTP)
* [ofxIO](https://github.com/bakercp/ofxIO)
* [ofxMediaType](https://github.com/bakercp/ofxMediaType)
* [ofxNetworkUtils](https://github.com/bakercp/ofxNetworkUtils)
* [ofxSSLManager](https://github.com/bakercp/ofxSSLManager)


## Instructions

Open an example and compile. When running it you must have your RunwayML app running, with the same model as the example in your workspace and running. The examples are named after each model.

For models that ingest images there are usually two examples: one which uses your computers webcam and continously streams frames to RunwayML, and the other one opens an image file and processes it once.


## Explanation

The Runway's app method of communication with other applications is using some network protocol. So far, HTTP is the one implemented.

ofxRunway takes charge of making the needed requests via HTTP on its own thread, as well as receiving the response and processing it when needed; if the response is an image it will decode it and put into an ofImage.

Runway expects to receive and return data in the JSON format.

The particular format for what is sent and received can be queried by calling
`http://localhost:8000/info` which will return a JSON file with the structure for the input and output packages.

### Clases

ofxRunway has the following classes 

#### **ofxRunwayListener**
  * This is an abstract class. 
  * The idea is that you extend your `ofApp` class by delcaring it like this
  `class ofApp : public ofBaseApp, public ofxRunwayListener`
  as well as adding and implementing the following callback functions:
	```cpp
	void runwayInfoEvent(ofJson& info){}
	void runwayErrorEvent(string& message){}
	```
   which will get called whenever needed by `ofxRunway`. You should use this as a mean for communication with `ofxRunway`
  * In order for these callbacks to work you need to setup your `ofxRunway` instance by calling `runway.setup(this, "http://localhost:8000");` asumming that you are calling it from within `ofApp` and the second parameter is the correct one.


####  **ofxRunway**

  * Is the main class, which is in charge of managing the sending and receiving of data.
  * All the HTTP calls are executed on a different thread, so it wont block your app while waiting for a response.
  *  It includes several handy functions for sending and receiving different data types.
  *  It is designed to be thread safe. You dont need to take care of mutexes.
  
##### **Methods**
######  setup
* Run any of these during setup or if, for some reason, you want to reset the connection during runtime. 
* Only use the version with two arguments when you have implemented your class extending `ofxRunwayListener` as described previously.
* You can get the host string from RunwayML app, which is usually defaulted to `"http://localhost:8000"`;
 
    ```cpp 
    bool setup(const string& host);
	 bool setup(ofxRunwayListener& listenerClass, const string& host);
	 bool setup(ofxRunwayListener* listenerClass, const string& host);   
	 ```

######  send
* send an `ofxRunwayData` instance with the necesary data by calling ` void send(ofxRunwayData & data);`
* This will create the call on its own thread, so it wont block your main app.
* There are also the following handy functions to be used when you only need to send a single item.
    ```cpp 
    bool send(const string& name, const ofBaseHasPixels& img, ofxRunwayImageType type =OFX_RUNWAY_JPG, int resize_width = 0, int resize_height = 0);
    bool send(const string& name, const ofPixels& pix, ofxRunwayImageType type = OFX_RUNWAY_JPG, int resize_width = 0, int resize_height = 0);
    bool send(const string& name, string& data);
    ```
######  tryReceive
## Contributing

🎉Community Contribution


This is still a work in progress. Contributions are welcomed!

## History
* v1. [Original addon](https://github.com/genekogan/ofxRunway) by [Gene Kogan](https://github.com/genekogan/) 🎉
* v2. Updated, with many examples by [Roy Macdonald](https://github.com/roymacdonald/) 🎉

Note: v1 and v2 are not compatible even though they share a lot of code.

