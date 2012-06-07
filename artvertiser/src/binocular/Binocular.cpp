//
//  Binocular.cpp
//  TheArtvertiserAndroid
//
//  Created by Damian Stewart on 11/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Binocular.h"

static Binocular* instance = NULL;

Binocular* Binocular::get() { 
	if ( instance == NULL )
		instance = new Binocular();
	return instance;
}

void Binocular::setup( ofVideoGrabber& grabber, bool bDebug )
{
	buttons.setup( bDebug );
	
	menu.setup( buttons );
	
	recorder.setup( grabber );
	
	ofAddListener( buttons.redAndBlueButtons, this, &Binocular::redAndBlueButtonsPressed );

	ofAddListener( ofEvents().keyPressed, this, &Binocular::keyPressed );
	
}


void Binocular::redAndBlueButtonsPressed( bool &pressed )
{
	if ( pressed )
		recorder.toggleRecording();
}

#ifdef TARGET_OSX
void Binocular::gotFrame(  ofPixels& pixels )
{
	recorder.addFrame( pixels );
}
#endif


void Binocular::keyPressed( ofKeyEventArgs& args )
{
	int key = args.key;
	bool t = true;
	if ( key == '1' )
	{
		ofNotifyEvent( buttons.redButton, t );
	}
	else if ( key == '2' )
	{
		ofNotifyEvent( buttons.greenButton, t );
	}
	else if ( key == '3' )
	{
		ofNotifyEvent( buttons.blueButton, t );
	}
	else if ( key == '4' )
	{
		ofNotifyEvent( buttons.redAndBlueButtons, t );
	}
	
}
