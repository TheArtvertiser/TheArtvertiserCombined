//
//  BinocularVideoRecorder.cpp
//  TheArtvertiserAndroid
//
//  Created by Damian Stewart on 16/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "BinocularVideoRecorder.h"

static const float RECORD_FPS = 25.0f;
static const float RECORD_TIMESTEP = 1.0f/RECORD_FPS;
static const float DEBOUNCE_TIMEOUT = 1.0f;

void BinocularVideoRecorder::setup( ofVideoGrabber& grabber )
{
	debounceTimer = 0;
	setWidthHeight( grabber.getWidth(), grabber.getHeight() );
	
#ifdef TARGET_ANDROID
	ofxAndroidVideoGrabber * androidGrabber = (ofxAndroidVideoGrabber*) grabber.getGrabber().get();
	ofAddListener(androidGrabber->newFrameE,this,&BinocularVideoRecorder::addFrame);
#elif defined (TARGET_LINUX)
	ofGstVideoGrabber * gstGrabber = (ofGstVideoGrabber*) grabber.getGrabber().get();
	ofGstVideoUtils * videoUtils = gstGrabber->getGstVideoUtils();
	ofAddListener(videoUtils->bufferEvent,this,&BinocularVideoRecorder::addFrame);
#endif

	//CodecID codec = CODEC_ID_PNG;
#ifndef TARGET_ANDROID
	exporter.setup( width, height, 4000000, RECORD_FPS, CODEC_ID_MPEG4, "mp4" );
	exporter.setPixelSource( grabber.getPixels(), grabber.getWidth(), grabber.getHeight() );
#endif
}

void BinocularVideoRecorder::toggleRecording()
{
	if ( !recording && debounceTimer < ofGetElapsedTimef() )
	{
		recording = true;
		ofLogNotice( "BinocularVideo", "starting recording");
		
		
		// go
		lastFrameTime = ofGetElapsedTimef()-RECORD_TIMESTEP;
		string filename = ofGetTimestampString()+"_";
		string folder = "recordings";
		snapshotted = false;
		snapshotFilename = folder + "/" + filename + ".mp4.png";
		
#ifndef TARGET_ANDROID
		exporter.record( filename, folder, true );
#endif

		ofAddListener( ofEvents().draw, this, &BinocularVideoRecorder::draw );
		
	}
	else
	{
		ofLogNotice( "BinocularVideo", "stopping recording");
		

		recording = false;

#ifndef TARGET_ANDROID
		exporter.stop();
#endif
		ofRemoveListener( ofEvents().draw, this, &BinocularVideoRecorder::draw );
		debounceTimer = ofGetElapsedTimef() + DEBOUNCE_TIMEOUT;
	}
}

void BinocularVideoRecorder::addFrame( ofPixels& pixels )
{
	if ( !recording )
		return;

#ifndef TARGET_ANDROID
	exporter.addFrame( pixels.getPixels(), pixels.getWidth(), pixels.getHeight() );
#endif
	lastFrameTime = ofGetElapsedTimef();
}

void BinocularVideoRecorder::draw(ofEventArgs &args)
{
	float alpha = sinf( ofGetElapsedTimef()*10.0f )*0.5f + 0.5f;
	ofSetColor( 255, 0, 0, 255.0f*alpha );
//	ofSetColor( 255, 0, 0 );
	ofCircle( ofGetWidth()-50, 50, 20 );
}


