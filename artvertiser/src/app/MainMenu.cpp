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

#include "MainMenu.h"
#include "ofAppRunner.h"
#include "PersistanceEngine.h"

MainMenu::MainMenu()
:circularPB(15)
{
}

void MainMenu::setup( bool doTakeAPhoto, bool doComms ){
	ofAddListener(ofEvents().windowResized,this,&MainMenu::windowResized);

	float menuWidth = 0;
	yOffset = 0;
	if ( doTakeAPhoto )
	{
		ofImage cameraIcon;
		cameraIcon.loadImage("icons/camera.png");

		cameraButton = ofPtr<gui::Button>(new gui::Button);
		cameraButton->setIcon(cameraIcon);
		ofAddListener(cameraButton->pressedE,this,&MainMenu::cameraPressed);
		menu.addWidget(cameraButton);
		
		menuWidth = max(menuWidth, cameraIcon.getWidth() );
		
	}
	
	if ( doComms )
	{
		ofImage downloadIcon("icons/download.png");

		downloadButton = ofPtr<gui::Button>(new gui::Button);
		downloadButton->setIcon(downloadIcon);
		ofAddListener(downloadButton->pressedE,this,&MainMenu::downloadPressed);
		menu.addWidget(downloadButton);

		menuWidth = max(menuWidth, downloadIcon.getWidth() );
	}
	
	ofImage downIcon("icons/yes.png");
	downButton = ofPtr<gui::Button>(new gui::Button);
	downButton->setIcon(downIcon);
	ofAddListener(downButton->pressedE, this, &MainMenu::downPressed );
	menu.addWidget(downButton);
	ofImage upIcon("icons/no.png");
	upButton = ofPtr<gui::Button>(new gui::Button);
	upButton->setIcon(upIcon);
	ofAddListener(upButton->pressedE, this, &MainMenu::upPressed );
	menu.addWidget(upButton);
	
	if ( menuWidth > 0 )
		menu.setPosition(ofPoint(ofGetWidth()-menuWidth-20,20));
	menu.setWidth(menuWidth);
	
	//grid.setPosition(ofPoint(20, 20));
	float cameraIconWidth = 90.0f;
	grid.setCellSize(menuWidth*1.4, menuWidth*1.4*3./4.);
	grid.setSpacing(20,20);
	float maxX = ofGetWidth();
	if ( doComms || doTakeAPhoto )
	{
		if ( doComms )
			maxX = downloadButton->getRect().x-5;
		else 
			maxX = cameraButton->getRect().x-5;
	}
	grid.setRectangle(ofRectangle(20,20, maxX,ofGetHeight()));
	refresh();
}

void MainMenu::windowResized(ofResizeEventArgs & window){
	if ( cameraButton )
	{
		menu.setPosition(ofPoint(ofGetWidth()-cameraButton->getRect().width-20,20));
		grid.setRectangle(ofRectangle(20,20,cameraButton->getRect().x-5,ofGetHeight()));
	}
	else if ( downloadButton )
	{
		menu.setPosition(ofPoint(ofGetWidth()-downloadButton->getRect().width-20,20));
		grid.setRectangle(ofRectangle(20,20,downloadButton->getRect().x-5,ofGetHeight()));
	}
	else {
		grid.setRectangle(ofRectangle(20,20,ofGetWidth(),ofGetHeight()));
	}
}

void MainMenu::enableEvents(){
	menu.enableEvents();
	grid.enableEvents();
}

void MainMenu::disableEvents(){
	//grid.clear();
	menu.disableEvents();
	grid.disableEvents();
}

void MainMenu::refresh(){
	artverts = Artvert::listAll();
	grid.clear();
	readyCache.resize(artverts.size());
	for(int i=0; i<(int)artverts.size(); ++i){
		
		ofPtr<gui::Button> button(new gui::Button);
		grid.addWidget(button);
		ofPtr<ofImage> resizedImg = iconCache->getResource(artverts[i].getCompressedImage().getAbsolutePath()+"Resized");
		if(!resizedImg->bAllocated()){
			resizedImg->setUseTexture(false);
			resizedImg->loadImage(artverts[i].getCompressedImage());
			resizedImg->resize(grid.getCellWidth(), grid.getCellHeight());
		}
		button->setFocusedIcon(*resizedImg);
		button->setPressedIcon(*resizedImg);
		if(!artverts[i].isReady() || !artverts[i].checkIntegrity()){
			ofPtr<ofImage> bwImage;
			bwImage = iconCache->getResource(artverts[i].getCompressedImage().getAbsolutePath()+"BW");
			if(!bwImage->bAllocated()){
				bwImage->setUseTexture(false);
				*bwImage = *resizedImg;
				bwImage->setImageType(OF_IMAGE_GRAYSCALE);
			}
			button->setIcon(*bwImage);
		}else{
			button->setIcon(*resizedImg);
		}
		ofAddListener(button->pressedE,this,&MainMenu::snapshotPressed);
	}
	PersistanceEngine::save();
}

void MainMenu::update(){
	circularPB.update();
	grid.update();
	menu.update();
	for(int i=0;i<(int)artverts.size();i++){
		readyCache[i] = artverts[i].isReady();
	}
}

void MainMenu::draw(){
	menu.draw();
	ofPushMatrix();
	ofTranslate(0, yOffset);
	grid.draw();
	for(int i=0; i< (int)grid.size(); i++){
		if(!readyCache[i]){
			gui::Button * button = (gui::Button*)grid[i].get();
			circularPB.setPosition(ofPoint(button->getRect().x+button->getRect().width*0.5,button->getRect().y+button->getRect().height*0.5));
			circularPB.draw();
		}
	}
	ofPopMatrix();
}

void MainMenu::downPressed(bool &pressed ){
	yOffset += ofGetHeight()/2;
}

void MainMenu::upPressed(bool &pressed ){
	yOffset -= ofGetHeight()/2;
}

void MainMenu::cameraPressed(bool & pressed){
	bool yes=true;
	ofNotifyEvent(cameraPressedE,yes);
}

void MainMenu::downloadPressed(bool & pressed){
	bool yes=true;
	ofNotifyEvent(downloadPressedE,yes);
}

void MainMenu::snapshotPressed(const void * sender, bool & pressed){
	cout <<  "snapshot pressed" << endl;
	for(int i=0;i<(int)grid.size();i++){
		if(grid[i].get()==sender){
			ofNotifyEvent(artvertSelectedE,artverts[i]);
		}
	}
}

void MainMenu::setIconCache(ofPtr<gui::IconCache> & _iconCache){
	iconCache = _iconCache;
}
