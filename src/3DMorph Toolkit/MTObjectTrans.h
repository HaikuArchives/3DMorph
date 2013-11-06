/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTObjectTranslator class, used in add-ons
 *
**/

#ifndef MTOBJECTTRANS_H
#define MTOBJECTTRANS_H

#include "MTObject.h"
#include <size_t.h>

class MTObjectTranslator : public MTObject
{
public:
	MTObjectTranslator();
	virtual ~MTObjectTranslator();
	
	// overridable functions
	virtual bool CanHandleType(const char *mimeType);
		// true if the given MIME type can be handled by the object
	
	virtual void Load(const char *mimeType, const void *data, size_t size);
		// load the data (do not free the data!)
};

#endif MTOBJECTTRANS_H