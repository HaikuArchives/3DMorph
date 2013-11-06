/*
 * 3D Morph Toolkit
 *
 *
 * "Simple" Add-on: just a white segment
 *
 * main file
 *
 */

#include "MyAddOn.h"

#include "MTAddOn.h"
#include "MTVector.h"
#include "MTPoints.h"
#include "MTMatrix.h"

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
	
	// create a segment from (-1,-1,-1) to (1,1,1) with 100 points
	*l_NewPoints = MTPoints::Segment(-1,-1,-1,1,1,1,100);
	
	// change its color to white (red=1, green=1, blue=1)
	l_NewPoints->SetColor(1,1,1);	
	
	return l_NewPoints;
}

// creates a configuration view
// NULL if no configuration
BView *MyAddOn::CreateView()
{
	return NULL;
}