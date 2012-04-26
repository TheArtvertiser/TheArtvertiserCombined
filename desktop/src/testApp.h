#pragma once

#include "ofMain.h"
#include "TakeAPhoto.h"
#include "ThreadedDetector.h"
#include "ofxGeoLocation.h"
#include "ofxGui.h"

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
		
		void newPhoto(Artvert & artvert);

		void showDialogPressed(bool & pressed);
		void photoFromDialog();

		TakeAPhoto photoApp;
		ThreadedDetector detector;
		ofImage currentImage;

		ofxPanel gui;
		ofxButton showDialog;

		enum State{
			WaitingPhoto,
			EnteringMetadata
		}state;

		ofFbo unwarpFbo;
		vector<ofPoint> srcQuad;
		ofPixels unwarpedPixels;
		ofxCvColorImage warpedImg;
		ofxCvColorImage unwarpImage;
};
