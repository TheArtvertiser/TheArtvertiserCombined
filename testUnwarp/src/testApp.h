#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Warping.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


		ofFbo unwarpFbo;
		vector<ofPoint> srcQuad;
		ofPixels unwarpedPixels;
		ofxCvColorImage warpedImg;
		ofxCvColorImage unwarpImage;
		vector<ofPoint> quad;
		gui::Warping warp;
		ofImage img;
		float scale;
		float videoWidth,videoHeight;
		bool showWarped;
		float w,h;
		ofDirectory dir;
		int currentImg;
		float aspectRatio;
};
