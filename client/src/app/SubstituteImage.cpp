//
//  SubstitueImage.cpp
//  openFrameworksLib
//
//  Created by Damian Stewart on 26.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "SubstituteImage.h"
#include "ofMain.h"

SubstituteImage::SubstituteImage()
{
	isVideo = false;
	
	imgQuad.resize(4);

}

SubstituteImage::~SubstituteImage()
{
	if ( isVideo )
		ofRemoveListener(  ofEvents().update, this, &SubstituteImage::update );
}

void SubstituteImage::setup( string filename )
{
	if ( isVideo )
		ofRemoveListener(  ofEvents().update, this, &SubstituteImage::update );

	ofLog(OF_LOG_NOTICE, "SubstituteImage:: setup for %s", filename.c_str() );
	
	isVideo = false;
	bool videoLoaded = false;
	bool imageLoaded = image.loadImage( filename );
	if ( !imageLoaded || image.getWidth() == 0 )
	{
		ofLog(OF_LOG_NOTICE, "image load for %s failed, trying video", filename.c_str() );
		videoLoaded = video.loadMovie( filename );
		if ( videoLoaded )
		{
			ofLog(OF_LOG_NOTICE, "loaded video, width/height %f %f", video.getWidth(), video.getHeight() );
			video.setUseTexture(true);
			isVideo = true;
		}
	}
		
	
	if ( isVideo )
		ofAddListener( ofEvents().update, this, &SubstituteImage::update );

	if ( isVideo )
	{
		imgQuad[0].set(0,0);
		imgQuad[1].set(video.getWidth(),0);
		imgQuad[2].set(video.getWidth(),video.getHeight());
		imgQuad[3].set(0,video.getHeight());
	}
	else
	{
		imgQuad[0].set(0,0);
		imgQuad[1].set(image.getWidth(),0);
		imgQuad[2].set(image.getWidth(),image.getHeight());
		imgQuad[3].set(0,image.getHeight());
	}		

}

void SubstituteImage::update( ofEventArgs& args )
{
	video.update();
}


void SubstituteImage::draw( float x, float y )
{
	if ( isVideo )
	{
		video.draw( x, y );
	}
	else
	{
		image.draw( x, y );
	}
	
}
