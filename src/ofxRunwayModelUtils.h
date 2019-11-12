//
//  ofxRunwayModelUtils.h
//  ofxRunway_BM-densecap
//
//  Created by Roy Macdonald on 11/8/19.
//
//

#include "ofMain.h"
namespace ofxRunway{
	enum PoseFeature{
	 POSE_NOSE_INDEX 			=  0,
	 POSE_LEFT_EYE_INDEX  		=  1,
	 POSE_RIGHT_EYE_INDEX 		=  2,
	 POSE_LEFT_EAR_INDEX  		=  3,
	 POSE_RIGHT_EAR_INDEX 		=  4,
	 POSE_LEFT_SHOULDER_INDEX  =  5,
	 POSE_RIGHT_SHOULDER_INDEX =  6,
	 POSE_LEFT_ELBOW_INDEX  	=  7,
	 POSE_RIGHT_ELBOW_INDEX 	=  8,
	 POSE_LEFT_WRIST_INDEX  	=  9,
	 POSE_RIGHT_WRIST_INDEX 	= 10,
	 POSE_LEFT_HIP_INDEX  		= 11,
	 POSE_RIGHT_HIP_INDEX 		= 12,
	 POSE_LEFT_KNEE_INDEX  	= 13,
	 POSE_RIGHT_KNEE_INDEX 	= 14,
	 POSE_LEFT_ANKLE_INDEX  	= 15,
	 POSE_RIGHT_ANKLE_INDEX 	= 16
	};


class ModelUtils {
 public:
	/**
	 * Traverses Pose Net poses and keypoints and draws ellipses for each keypoint position
	 * @param data - the Runway Pose JSON object
	 * @param g	   - the PGraphics instance to draw into
	 * @param ellipseSize - dimensions in pixels of each keypoint position ellipse diameter
	 */
	static void drawPoseParts(ofJson data,PGraphics g,float ellipseSize){
		try{
			// Only if there are any humans detected
			if (data != null) {
				JSONArray humans = data.getJSONArray("poses");
				for(int h = 0; h < humans.size(); h++) {
					JSONArray keypoints = humans.getJSONArray(h);
					// Now that we have one human, let's draw its body parts
					for (int k = 0; k < keypoints.size(); k++) {
						// Body parts are relative to width and weight of the input
						JSONArray point = keypoints.getJSONArray(k);
						float x = point.getFloat(0);
						float y = point.getFloat(1);
						g.ellipse(x * g.width, y * g.height, ellipseSize, ellipseSize);
					}
				}
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}
};
}
