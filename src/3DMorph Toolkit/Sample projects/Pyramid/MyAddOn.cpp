/*
 * 3D Morph Toolkit
 *
 *
 * Pyramid Add-on
 *
 * this sample code shows how to use the MTPoints and MTMatrix classes
 *
 * main file
 *
 */

#include "MyAddOn.h"

#include "MTAddOn.h"
#include "MTVector.h"
#include "MTPoints.h"
#include "MTMatrix.h"
#include <GraphicsDefs.h>	// for rgb_color

// Add-on exported symbols
/// add-on type
MTAddOnType addOnType = MT_TOOLKIT;

/// object instantiation function
MTObject *InstantiateObject()
{
	// create a new object
	return new MyAddOn;
}

// ***
// *** MyAddOn class
// ***

// constructor
MyAddOn::MyAddOn()
{
}

// destructor
MyAddOn::~MyAddOn()
{
}

// generate a new set of points
// musn't return NULL!
MTPoints *MyAddOn::Update(float)
{
	MTPoints *l_NewPoints = new MTPoints;

	// add your modifications to l_NewPoints here!
	
	// first we will create a small ball, one of the 4 that will make the 
	// pyramid
	MTPoints l_Ball;
	
	/// a ball is made of 3 circles, on in the X-Y plane, one in the Y-Z plane,
	/// and one in the Z-X plane
	/// The MTPoints::Circle function creates a circle in the X-Y plane.
	/// We will make one with 60 points
	MTPoints l_Circle = MTPoints::Circle(60);
	
	/// this first circle in the X-Y plane can be added to the ball
	/// (& is the concatenation operator)
	l_Ball &= l_Circle;
	
	/// next we add the circle in the Y-Z plane, which is obtained by rotating
	/// l_circle around the Y axis by pi/2.
	/// To do this rotation, we use a RotationY matrix:
	l_Ball &= l_Circle * MTMatrix::RotateY(3.1415/2);
	
	/// and now we add the circle in the Z-X plane, obtained by rotating
	/// l_circle around the X axis by pi/2
	l_Ball &= l_Circle * MTMatrix::RotateX(3.1415/2);
	
	/// the ball is now complete, but we need a smaller one. So we'll scale it
	/// we also want to rotate it a bit so that it doesn't look aligned with
	/// the others.
	/// we need to do several transformations here, so we'll first create a
	/// transformtation matrix, and then we'll apply it to the ball
	MTMatrix l_TransformationMatrix = MTMatrix::Scale(0.2);
	l_TransformationMatrix *= MTMatrix::RotateX(3.1415/4);
	l_TransformationMatrix *= MTMatrix::RotateY(3.1415/4);
	
	l_Ball *= l_TransformationMatrix;
	
	// here we are... now we will be able to add the balls to l_NewPoints
	/// first define a few coordinates
	static const float l_Ybottom = -0.272;
	static const float l_Ytop = 0.544;
	static const float l_Zfore = -0.289;
	static const float l_Zback = 0.577;
	static const float l_Xright = 0.5;
	static const float l_Xleft = -0.5;
	/// and a few colors
	static const rgb_color l_Orange = { 255,168,0,0};
	static const rgb_color l_Blue = { 0,96,255,0};
	static const rgb_color l_Purple = { 226, 25, 191,0};
	static const rgb_color l_Green = { 18, 158, 28, 0};
	
	/// first ball: orange, bottom, left, foreground
	l_Ball.SetColor(l_Orange);
	*l_NewPoints &= l_Ball * MTMatrix::Translate(l_Xleft,l_Ybottom,l_Zfore);
	
	/// second ball: blue, bottom, right, foreground
	l_Ball.SetColor(l_Blue);
	*l_NewPoints &= l_Ball * MTMatrix::Translate(l_Xright,l_Ybottom,l_Zfore);
	
	/// third ball: purple, bottom, center, background
	l_Ball.SetColor(l_Purple);
	*l_NewPoints &= l_Ball * MTMatrix::Translate(0,l_Ybottom,l_Zback);
	
	/// fourth ball: green, top
	l_Ball.SetColor(l_Green);
	*l_NewPoints &= l_Ball * MTMatrix::Translate(0,l_Ytop,0);
	
	// and add the segments between the balls
	// each segment starts at the center of a ball, and goes to the center
	// of another one. It's color is a gradient bewteen the colors of the
	// two balls
	MTPoints l_Segment;
	
	/// from the orange ball to the blue one
	//// create the segment
	l_Segment = MTPoints::Segment(l_Xleft,l_Ybottom,l_Zfore,
		l_Xright,l_Ybottom,l_Zfore,50);
	//// change its color
	l_Segment.SetColor(l_Orange,l_Blue);
	//// add it to the points
	*l_NewPoints &= l_Segment;
	
	/// blue -> purple 
	l_Segment = MTPoints::Segment(l_Xright,l_Ybottom,l_Zfore,
		0,l_Ybottom,l_Zback,50);
	l_Segment.SetColor(l_Blue,l_Purple);
	*l_NewPoints &= l_Segment;
	
	/// purple -> orange 
	l_Segment = MTPoints::Segment(0,l_Ybottom,l_Zback,
		l_Xleft,l_Ybottom,l_Zfore,50);
	l_Segment.SetColor(l_Purple,l_Orange);
	*l_NewPoints &= l_Segment;
	
	/// orange -> green
	l_Segment = MTPoints::Segment(l_Xleft,l_Ybottom,l_Zfore,
		0,l_Ytop,0,50);
	l_Segment.SetColor(l_Orange,l_Green);
	*l_NewPoints &= l_Segment;
	
	/// blue -> green
	l_Segment = MTPoints::Segment(l_Xright,l_Ybottom,l_Zfore,
		0,l_Ytop,0,50);
	l_Segment.SetColor(l_Blue,l_Green);
	*l_NewPoints &= l_Segment;
	
	/// purple -> green 
	l_Segment = MTPoints::Segment(0,l_Ybottom,l_Zback,
		0,l_Ytop,0,50);
	l_Segment.SetColor(l_Purple,l_Green);
	*l_NewPoints &= l_Segment;
		
	// finished!
	return l_NewPoints;
}

// creates a configuration view
// NULL if no configuration
BView *MyAddOn::CreateView()
{
	return NULL;
}