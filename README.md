<p align="center">
  <img src="./assets/cover_runway_openframeworks_github.jpg">
</p>

#  ofxRunway
### RunwayML + openFrameworks

[openFrameworks](http://openframeworks.cc/) addon that allows you to easily integrate  [RunwayML](https://runwayml.com/) into your openFrameworks App. 
It contains examples on most of the currently available models.

##Install
Download and copy or `git clone` this addon into your openFrameworks addons folder.

Generate or update the example projects by dragging their folder onto the projectGenerator and hitting "update" or "generate".

##Dependencies

You'll need to install the following addons for this addon to work.

* [ofxHTTP](https://github.com/bakercp/ofxHTTP)
* [ofxIO](https://github.com/bakercp/ofxIO)
* [ofxMediaType](https://github.com/bakercp/ofxMediaType)
* [ofxNetworkUtils](https://github.com/bakercp/ofxNetworkUtils)
* [ofxSSLManager](https://github.com/bakercp/ofxSSLManager)


## Instructions

Open an example and compile. When running it you must have your RunwayML app running, with the same model as the example on your workspace and running. The examples are named after each model.

For models that ingest images there are usualy two examples, one which uses your computers webcam and continously streams frames to Runway. The other one opens an image file and processes it once.


## Contributing

🎉Community Contribution


This is still a work in progress. Contributions are welcomed!

##History
* v1. [Original addon](https://github.com/genekogan/ofxRunway) by [Gene Kogan](https://github.com/genekogan/)
* v2. Updated, improoved and a lot of examples by [Roy Macdonald](https://github.com/roymacdonald/)

Note: v1 and v2 are not compatible even when these share a lot of code.

