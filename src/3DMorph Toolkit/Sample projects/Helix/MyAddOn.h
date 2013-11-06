/*
 * 3D Morph Toolkit
 *
 *
 * Helix Add-on
 *
 * This sample shows how to use the MTVector class, and how to make a
 * configuration view
 *
 * main header
 */

#ifndef MYADDON_H
#define MYADDON_H

#include "MTObject.h"
#include <GraphicsDefs.h>

class MyAddOn : public MTObject
{
public:
	// constructor and destructor
	MyAddOn();
	virtual ~MyAddOn();
	
	// overrided functions
	virtual MTPoints *Update(float);	
			// generate a new set of points. Ignore the float for now. It will
			// be useful later
			// the MTPoints object becomes the responsability of the application
			// the add-on musn't use or delete it after this call
			// mustn't return NULL
	virtual BView *CreateView();
			// creates a view to configure the add-on
			// returns NULL if no configuration
	
	// object parameters
	unsigned int m_NumLines;	// number of lines (1..32)
	float m_Twist;				// twisting amount (0..1)
	float m_Diameter;			// diameter (0..1)
	float m_Length;				// length (0..1)
	rgb_color m_Color1,m_Color2;	// colors
};

#endif //MYADDON_H