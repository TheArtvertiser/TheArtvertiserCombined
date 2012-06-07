//
//  SubstitueImage.h
//  openFrameworksLib
//
//  Created by Damian Stewart on 26.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class SubstituteImage
{
public:
	SubstituteImage();
	~SubstituteImage();
	
	void setup( string filename );
	
	void update( ofEventArgs & args );
	
	void draw( float x, float y );
	
	/// call to turn on/off movie playback
	void enable();
	void disable();
	
	const vector<ofPoint>& getImgQuad() { return imgQuad; };
	
private:
	
	ofImage image;
	ofVideoPlayer video;
	
	bool isVideo;
	bool videoWasDrawn;
	
	vector <ofPoint> imgQuad;
	
};

