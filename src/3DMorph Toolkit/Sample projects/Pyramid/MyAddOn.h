/*
 * 3D Morph Toolkit
 *
 *
 * Add-on
 *
 * main header
 */

#ifndef MYADDON_H
#define MYADDON_H

#include "MTObject.h"

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
};

#endif //MYADDON_H