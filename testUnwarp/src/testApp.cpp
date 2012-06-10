#include "testApp.h"
#include "homography.h"
//--------------------------------------------------------------
void testApp::setup(){
	dir.allowExt("jpg");
	dir.allowExt("JPG");
	dir.listDir("");
	currentImg = 0;
	img.loadImage(dir.getFile(0).path());
	if(img.getWidth()>img.getHeight())
		img.resize(640,480);
	else
		img.resize(480,640);
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

double sqr( double x)
{
	return x*x;
}

/// originalRoiPoints are the original (unwarped, perspective distorted) points
/// cameraPrincipalPoint is the centre of barrel distortion, usually (imageWidth/2,imageHeight/2)
float calculateAspectRatio( const vector<ofPoint> &originalRoiPoints, ofVec2f cameraPrincipalPoint )
{
	float m1x = originalRoiPoints[0].x;
	float m1y = originalRoiPoints[0].y;
	float m2x = originalRoiPoints[1].x;
	float m2y = originalRoiPoints[1].y;
	float m3x = originalRoiPoints[3].x;
	float m3y = originalRoiPoints[3].y;
	float m4x = originalRoiPoints[2].x;
	float m4y = originalRoiPoints[2].y;
	
	float u0 = cameraPrincipalPoint.x;
	float v0 = cameraPrincipalPoint.y;
	
	// in case it matters: licensed under GPLv2 or later
	// legend:
	// sqr(x)  = x*x
	// sqrt(x) = square root of x
	
	// let m1x,m1y ... m4x,m4y be the (x,y) pixel coordinates
	// of the 4 corners of the detected quadrangle
	// i.e. (m1x, m1y) are the cordinates of the first corner, 
	// (m2x, m2y) of the second corner and so on.
	// let u0, v0 be the pixel coordinates of the principal point of the image
	// for a normal camera this will be the center of the image, 
	// i.e. u0=IMAGEWIDTH/2; v0 =IMAGEHEIGHT/2
	// This assumption does not hold if the image has been cropped asymmetrically
	
	// first, transform the image so the principal point is at (0,0)
	// this makes the following equations much easier
	m1x = m1x - u0;
	m1y = m1y - v0;
	m2x = m2x - u0;
	m2y = m2y - v0;
	m3x = m3x - u0;
	m3y = m3y - v0;
	m4x = m4x - u0;
	m4y = m4y - v0;
	
	u0 = 0;
	v0 = 0;
	
	// temporary variables k2, k3
	double k2 = ((m1y - m4y)*m3x - (m1x - m4x)*m3y + m1x*m4y - m1y*m4x) /
	((m2y - m4y)*m3x - (m2x - m4x)*m3y + m2x*m4y - m2y*m4x) ;
	
	double k3 = ((m1y - m4y)*m2x - (m1x - m4x)*m2y + m1x*m4y - m1y*m4x) / 
	((m3y - m4y)*m2x - (m3x - m4x)*m2y + m3x*m4y - m3y*m4x) ;
	

	
	// f_squared is the focal length of the camera, squared
	// if k2==1 OR k3==1 then this equation is not solvable
	// if the focal length is known, then this equation is not needed
	// in that case assign f_squared= sqr(focal_length)
	//double f_squared =  -((k3*m3y - m1y)*(k2*m2y - m1y) + (k3*m3x - m1x)*(k2*m2x - m1x)) / 	((k3 - 1)*(k2 - 1)) ;
	//ofLogNotice() << " k2 k3 " << k2 << " " << k3;
	//double f_squared = sqr(u0); // (width/2)^2
	//double f_squared = sqr(0.5f);
	double f_squared = sqr(45.2097243131986);

	
	//The width/height ratio of the original rectangle
	double whRatioPre = 
	(sqr(k2 - 1) + sqr(k2*m2y - m1y)/f_squared + sqr(k2*m2x - m1x)/f_squared) /
	(sqr(k3 - 1) + sqr(k3*m3y - m1y)/f_squared + sqr(k3*m3x - m1x)/f_squared) ;
	double whRatio = sqrt( fabs(whRatioPre)  ) ;
	//ofLogNotice() << " whRatioPre " << whRatioPre << " whRatio " << whRatio;
	
	

	// if k2==1 AND k3==1, then the focal length equation is not solvable 
	// but the focal length is not needed to calculate the ratio.
	// I am still trying to figure out under which circumstances k2 and k3 become 1
	// but it seems to be when the rectangle is not distorted by perspective, 
	// i.e. viewed straight on. Then the equation is obvious:
	if (k2==1 && k3==1) 
		whRatio = sqrt( (sqr(m2y-m1y) + sqr(m2x-m1x)) / 
					   (sqr(m3y-m1y) + sqr(m3x-m1x)) );
	



	// After testing, I found that the above equations 
	// actually give the height/width ratio of the rectangle, 
	// not the width/height ratio. 
	// If someone can find the error that caused this, 
	// I would be most grateful.
	// until then:
	//whRatio = 1.0f/whRatio;
	
	return whRatio;
}


//--------------------------------------------------------------
void testApp::update(){
	warpedImg.setFromPixels(img.getPixelsRef());
	unwarpImage.warpIntoMe(warpedImg,&warp.getQuad()[0],&srcQuad[0]);
	
	/*
	w = MIN(warp.getQuad()[0].distance(warp.getQuad()[1]),warp.getQuad()[2].distance(warp.getQuad()[3]));
	h = MIN(warp.getQuad()[0].distance(warp.getQuad()[3]),warp.getQuad()[1].distance(warp.getQuad()[2]));*/
	
	aspectRatio = calculateAspectRatio( warp.getQuad(), ofVec2f(img.getWidth()/2,img.getHeight()/2) );

	unwarpFbo.begin();
	ofClear(0);
	if(aspectRatio>=4./3.){
		w = 640;
		h = 640/aspectRatio;
	}
	else{
		h = 480;
		w = 480*aspectRatio;
	}
	//

	unwarpImage.draw(0,0,w,h);
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
		if(img.getWidth()>img.getHeight())
			img.resize(640,480);
		else
			img.resize(480,640);
	}
	if(key==OF_KEY_DOWN){
		currentImg--;
		currentImg%=dir.size();
		img.loadImage(dir.getFile(currentImg).path());
		if(img.getWidth()>img.getHeight())
			img.resize(640,480);
		else
			img.resize(480,640);
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
	ofLogNotice() << "aspect ratio: " << aspectRatio << " w/h " << w << "," << h ;
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
