#include "testApp.h"
#include "homography.h"

//--------------------------------------------------------------
void testApp::setup(){
	state  = WaitingPhoto;
	ofBackground(66,51,51);
	ofEnableAlphaBlending();
	ofAddListener(photoApp.newPhotoE,this,&testApp::newPhoto);

	gui.setup();
	gui.add(showDialog.setup("open picture"));

	showDialog.addListener(this,&testApp::showDialogPressed);

	srcQuad.resize(4);
	srcQuad[0].set(0,0);
	srcQuad[1].set(640,0);
	srcQuad[2].set(640,480);
	srcQuad[3].set(0,480);
	unwarpFbo.allocate(640,480,GL_RGB);
	unwarpImage.allocate(640,480);

	detector.setup("adverts","artverts");

}

//--------------------------------------------------------------
void testApp::update(){
	if(state==EnteringMetadata) photoApp.update();
	detector.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	if(state==EnteringMetadata){
		photoApp.draw();
	}else{
		gui.draw();
		if(detector.getState()==Detector::Training){
			ofDrawBitmapString("training " + detector.getCurrentArtvert().getAdvertName(),ofGetWidth()-300,20);
			detector.getCurrentImage().draw(ofGetWidth()-300,40,240,160);
			queue<Artvert> pendingArtverts = detector.getPending();
			int i=0;
			while(!pendingArtverts.empty()){
				ofDrawBitmapString(pendingArtverts.front().getAdvertName(),ofGetWidth()-300,220+20*i);
				pendingArtverts.pop();
				i++;
			}
		}else{
			ofDrawBitmapString("waiting",ofGetWidth()-100,20);
	}
	}
}

//--------------------------------------------------------------
void testApp::newPhoto(Artvert & artvert){
	artvert.save();
	photoApp.stop();
	state = WaitingPhoto;
	ofImage img(artvert.getCompressedImage());
	vector<ofPoint> quad = artvert.getROI();
	warpedImg.setFromPixels(img.getPixelsRef());
	unwarpImage.warpIntoMe(warpedImg,&srcQuad[0],&quad[0]);
	/*ofMatrix4x4 H = findHomography(&quad[0],&srcQuad[0]);
	unwarpFbo.begin();
	glMultMatrixf(H.getPtr());
	img.draw(0,0);
	unwarpFbo.end();
	unwarpFbo.readToPixels(unwarpedPixels);*/

	ofSaveImage(unwarpImage.getPixelsRef(),"adverts/"+artvert.getUID()+".unwarped.png");
	detector.addAdvertForTraining(artvert);//setupTrainOnly(artvert.getModel().path());
}

//--------------------------------------------------------------
void testApp::showDialogPressed(bool & pressed){
	if(!pressed) photoFromDialog();
}

//--------------------------------------------------------------
void testApp::photoFromDialog(){
	ofFileDialogResult res = ofSystemLoadDialog("load a picture",false);
	if(!res.bSuccess) return;

	string path = res.filePath;
	currentImage.loadImage(path);
	photoApp.setup(currentImage,true);
	photoApp.start();
	state = EnteringMetadata;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
