#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double sqr( double x)
{
	return x*x;
}

/// originalRoiPoints are the original (unwarped, perspective distorted) points
/// cameraPrincipalPoint is the centre of barrel distortion, usually (imageWidth/2,imageHeight/2)
float calculateAspectRatio( float originalRoiPoints[8], float u0, float v0 )
{
	float m1x = originalRoiPoints[0];
	float m1y = originalRoiPoints[1];
	float m2x = originalRoiPoints[2];
	float m2y = originalRoiPoints[3];
	float m3x = originalRoiPoints[6];
	float m3y = originalRoiPoints[7];
	float m4x = originalRoiPoints[4];
	float m4y = originalRoiPoints[5];
	
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

void usage(const char* argv0)
{
	fprintf(stderr, "usage: %s x1 y1 x2 y2 x3 y3 x4 y4 u0 v0\n", argv0 );
	fprintf(stderr, "  x1,y1-x4,y4: ROI (clockwise)\n"
			        "  u0/v0: camera principal point (usually w/2,h/2)\n" );

}

int main( int argc, const char** argv )
{
	float roi[8];
	float u0, v0;
	if ( argc < 10 )
	{
		usage( argv[0] );
		exit(1);
	}

	int i;
	for ( i=0; i<8; i++ )
	{
		roi[i] = atof( argv[i+1] );
	}
	u0 = atof(argv[9]);
	v0 = atof(argv[10]);

	float aspectRatio = calculateAspectRatio( roi, u0, v0 );
	printf("%f\n", aspectRatio );
	return 0;
}


