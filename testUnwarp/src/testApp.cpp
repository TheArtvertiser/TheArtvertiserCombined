#include "testApp.h"
#include "homography.h"
//--------------------------------------------------------------
void testApp::setup(){
	dir.listDir("");
	currentImg = 0;
	img.loadImage(dir.getFile(0).path());
	quad.resize(4);
	quad[0].set(0,0);
	quad[1].set(640,0);
	quad[2].set(640,480);
	quad[3].set(0,480);
	warpedImg.allocate(640,480);
	unwarpImage.allocate(640,480);
	srcQuad = quad;
	scale = ofGetHeight()/img.getHeight();
	videoWidth = img.getWidth()*scale;
	videoHeight = img.getHeight()*scale;
	warp.setInitialQuad(quad,ofPoint(ofGetWidth()*0.5-videoWidth*0.5,ofGetHeight()*0.5-videoHeight*0.5),ofPoint(scale,scale));
	warp.setMinDistance(ofGetWidth()*ofGetHeight()*.00015);
	warp.enableEvents();
	unwarpFbo.allocate(640,480,GL_RGB);
	showWarped = false;
	ofDirectory warpedDir("warped");
	warpedDir.create();
}

//--------------------------------------------------------------
void testApp::update(){
	warpedImg.setFromPixels(img.getPixelsRef());
	unwarpImage.warpIntoMe(warpedImg,&warp.getQuad()[0],&srcQuad[0]);
	w = MIN(warp.getQuad()[0].distance(warp.getQuad()[1]),warp.getQuad()[2].distance(warp.getQuad()[3]));
	h = MIN(warp.getQuad()[0].distance(warp.getQuad()[3]),warp.getQuad()[1].distance(warp.getQuad()[2]));


	unwarpFbo.begin();
	ofClear(0);
	if(w>h){
		unwarpImage.draw(0,0,640,640.*h/w);
	}else{
		unwarpImage.draw(0,0,480*w/h,480);
	}
	unwarpFbo.end();
	unwarpFbo.readToPixels(unwarpedPixels);
}

//--------------------------------------------------------------
void testApp::draw(){
	//unwarpImage.draw(0,0);
	if(showWarped) unwarpFbo.draw(0,0);
	else warpedImg.draw(0,0);

	warp.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key==' '){
		showWarped = !showWarped;
	}
	if(key=='s'){
		string name = "warped/"+ofSystemTextBoxDialog("","")+".jpg";
		//unwarpedPixels = unwarpImage.getPixelsRef();
		unwarpedPixels.crop(0,0,640,640.*h/w);
		ofSaveImage(unwarpedPixels,name);
	}
	if(key==OF_KEY_UP){
		currentImg++;
		currentImg%=dir.size();
		img.loadImage(dir.getFile(currentImg).path());
	}
	if(key==OF_KEY_DOWN){
		currentImg--;
		currentImg%=dir.size();
		img.loadImage(dir.getFile(currentImg).path());
	}
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
