/*
 Copyright 2011 Arturo Castro <arturo@arturocastro.net>,

 Distributed under the terms of the GNU General Public License v3.

 This file is part of The Artvertiser for Android.

 The Artvertiser for Android is free software: you can redistribute
 it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.

 The Artvertiser for Android is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with The Artvertiser for Android.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ArtvertInfo.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "Button.h"

ArtvertInfo::ArtvertInfo()
:advert(new ofImage)
,refresh(false)
{
	// TODO Auto-generated constructor stub

}

ArtvertInfo::~ArtvertInfo() {
	// TODO Auto-generated destructor stub
}

void ArtvertInfo::setGeo(ofPtr<ofxGeoLocation> & _geo){
	geo = _geo;
}

void ArtvertInfo::show(Artvert & _artvert){
	mutex.lock();
	artvert = _artvert;

	location = artvert.getLocation();
	geo->setSize(ofGetWidth()/3.,ofGetWidth()/4.);
	geo->setLocation(location);

	ofImage cameraIcon;
	cameraIcon.loadImage("icons/camera.png");

	advert = iconCache->getResource(artvert.getCompressedImage().getAbsolutePath()+"Preview");
	if(!advert->bAllocated()){
		advert->setUseTexture(false);
		advert->loadImage(artvert.getCompressedImage());
		float ratio = advert->getHeight()/advert->getWidth();
		advert->resize(ofGetWidth()/3.,ofGetWidth()/3.*ratio);
	}
	quad.clear();
	quad.addVertexes(artvert.getROI());
	for(int i=0;i<quad.size();i++){
		quad[i].x *= ofGetWidth()/3. / 640;
		quad[i].y *= ofGetWidth()/3. / 640;
		quad[i].x += 20;
		quad[i].y += 20;
	}
	quad.close();

	grid.clear();
	grid.setRectangle(ofRectangle(ofGetWidth()/3.+40,50,ofGetWidth()-(ofGetWidth()/3.+60),ofGetHeight()));
	grid.setCellSize(cameraIcon.getWidth()*1.5, cameraIcon.getWidth()*1.5*3./4.);
	grid.setSpacing(20,20);
	grid.enableEvents();
	
	ofLogLevel oldLevel = ofGetLogLevel();

	artverts = artvert.getArtverts();
	ofLogVerbose("ArtvertiserApp", "number of artverts: "+ofToString(artverts.size()) );
	for(int i=0;i<(int)artverts.size();i++){
		ofLogVerbose("ArtvertiserApp", ofToString(i)+": "+artverts[i].getAbsolutePath() );

		grid.addWidget(ofPtr<gui::Button>(new gui::Button));
		gui::Button * button = (gui::Button*)grid.getWidget(i).get();
		ofPtr<ofImage> icon = iconCache->getResource(artverts[i].getAbsolutePath()+"Resized");
		if(!icon->bAllocated()){
			icon->setUseTexture(false);
			bool imageLoaded = icon->loadImage(artverts[i]);
#ifndef TARGET_ANDROID
			if ( !imageLoaded ){
				// try a video?
				ofVideoPlayer player;
				bool videoLoaded = player.loadMovie(artverts[i].getAbsolutePath());
				if ( videoLoaded ){
					// seek to middle frame
					player.setSpeed(0);
					player.play();
					player.setFrame( player.getTotalNumFrames()/2 );
					while (! player.isFrameNew() )
						player.update();
					// grab
					icon->setFromPixels( player.getPixelsRef() );
				}
			}
#endif
			float ratio = icon->getWidth()/icon->getHeight();
			float width,height;
			if(ratio>1){
				width = grid.getCellWidth();
				height = width/ratio;
			}else{
				height = grid.getCellHeight();
				width = height * ratio;
			}
			icon->resize(width,height);
		}
		button->setIcon(*icon);
		ofAddListener(button->pressedE,this,&ArtvertInfo::artvertPressed);
	}


	refresh = true;
	mutex.unlock();
}

void ArtvertInfo::stop(){
	grid.disableEvents();
	advert->setUseTexture(false);
}

void ArtvertInfo::update(){
	if(refresh){
		advert->setUseTexture(true);
		advert->reloadTexture();
		refresh = false;
	}
	grid.update();
	geo->update();
}

void ArtvertInfo::draw(){
	mutex.lock();
	advert->draw(20,20);

	ofNoFill();
	ofSetColor(0,255,0);
	quad.draw();

	ofFill();
	ofSetColor(255,255,255);
	geo->draw(20,ofGetHeight()-20-geo->getHeight());

	if(artvert.isReady())
		ofDrawBitmapString("Ready",advert->getWidth()+40,30);
	else
		ofDrawBitmapString("Waiting analisys",advert->getWidth()+40,30);

	ofDrawBitmapString(geo->getAddress() + ", " + geo->getCity() + " (" +geo->getCountry() +")",20, advert->getHeight()+40);

	grid.draw();
	mutex.unlock();
}

void ArtvertInfo::artvertPressed(const void * sender, bool & pressed){
	ofLogVerbose("ArtvertiserApp", "ArtvertInfo::artvertPressed");
	if(!artvert.isReady()) {
		ofLogVerbose("ArtvertiserApp", "artvert is not ready: returning");
		return;
	}
	for(int i=0;i<grid.size();i++){
		if(grid[i].get() == sender){
			ofLogVerbose("ArtvertiserApp", "grid "+ofToString(i)+" is sender, notifying");
			ofNotifyEvent(artvertSelectedE,artverts[i]);
			return;
		}
	}
}

Artvert & ArtvertInfo::getCurrentArtvert(){
	return artvert;
}

void ArtvertInfo::setIconCache(ofPtr<gui::IconCache> & _iconCache){
	iconCache = _iconCache;
}
