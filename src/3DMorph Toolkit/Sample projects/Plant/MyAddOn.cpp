/*
 * 3D Morph Toolkit
 *
 *
 * Add-on
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
	
	return l_NewPoints;
}

// creates a configuration view
// NULL if no configuration
BView *MyAddOn::CreateView()
{
	return NULL;
}