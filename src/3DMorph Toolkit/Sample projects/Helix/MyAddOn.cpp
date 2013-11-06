/*
 * 3D Morph Toolkit
 *
 *
 * Helix Add-on
 *
 * This sample shows how to use the MTVector class, and how to make a
 * configuration view
 *
 * main file
 *
 */

#include "MyAddOn.h"

#include "MTAddOn.h"
#include "MTVector.h"
#include "MTPoints.h"
#include "MTMatrix.h"

#include "HConfigView.h"

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
	static const rgb_color c_white = { 255,255,255,0};
	
	m_NumLines = 1;
	m_Diameter = 0.5;
	m_Length = 0.5;
	m_Twist = 0;
	
	m_Color1 = c_white;
	m_Color2 = c_white;
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

	// the formula used to build the helix is as follow:
	// for each line:
	// x = DIAM * cos( TWIST * 10 * t + PHI)
	// y = DIAM * sin( TWIST * 10 * t + PHI)
	// z = 2 * LENGTH * (t-0.5)
	// with t going from 0 to 1
	//      DIAM   = helix diameter
	//      TWIST  = twisting amount
	//      LENGTH = helix length
	//      PHI    = line starting phase:
	//             = 2 * pi * (i/n)
	//				 where N is the total number of lines,
	//                 and I is the line number (between 0 and n-1)
	//
	// the number of points used for each line is 1024/N
	
	// constants definition
	const unsigned int l_nPoints = 1024/m_NumLines;
	
	// variables
	float l_DeltaPhi = 6.2830 / m_NumLines;
		// variation of PHI between two lines
	MTVector l_Vect = MTVector::GradientExc(0,m_Twist*10,l_nPoints);
		// vector used in the cos and sin:
		// goes from TWIST*10*0 to TWIST*10*1
	// color vectors
	MTVector l_R = MTVector::GradientInc(m_Color1.red/255.0,
		m_Color2.red/255.0,l_nPoints);
	MTVector l_G = MTVector::GradientInc(m_Color1.green/255.0,
		m_Color2.green/255.0,l_nPoints);
	MTVector l_B = MTVector::GradientInc(m_Color1.blue/255.0,
		m_Color2.blue/255.0,l_nPoints);
		
	
	// lines loop
	for (unsigned int i=0; i<m_NumLines; i++)
	{
		// X vector
		MTVector l_X = cos(l_Vect) * m_Diameter;
		
		// Y vector
		MTVector l_Y = sin(l_Vect) * m_Diameter;
		
		// Z vector
		MTVector l_Z = MTVector::GradientInc(-m_Length,m_Length,l_nPoints);
		
		// add the line to the other ones
		*l_NewPoints &= MTPoints(l_X,l_Y,l_Z,l_R,l_G,l_B);
		
		// update the l_Vect vector for next line
		l_Vect += l_DeltaPhi;
	}
	  
	return l_NewPoints;
}

// creates a configuration view
// NULL if no configuration
BView *MyAddOn::CreateView()
{
	return new HConfigView(this);
}