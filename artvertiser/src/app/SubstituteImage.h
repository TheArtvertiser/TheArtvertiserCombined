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
	
	/// if preserveRatio is true the ratio will be preserved. 
	/// at bleed=1 image will be enlarged to fill the surface, and not cropped. 
	/// at bleed=0 image will shrink and black bars will be drawn.
	void setup( string filename, bool preserveRatio=true, float bleed=1.0f );
	
	void update( ofEventArgs & args );
	
	void draw( float x, float y );
	
	/// call to turn on/off movie playback
	void enable();
	void disable();
	
	const vector<ofPoint>& getImgQuad() { return imgQuad; };
	
private:
	
	ofImage image;
	ofVideoPlayer video;

	bool preserveRatio;
	float bleed;

	bool isVideo;
	bool videoWasDrawn;
	
	vector <ofPoint> imgQuad;
	
};

