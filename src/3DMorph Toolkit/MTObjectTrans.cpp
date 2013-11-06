/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTObjectTranslator class, used in add-ons
 *
 * default functions
 *
**/

#include "MTObjectTrans.h"
#include "Mainwin.h"
#include "MTPoints.h"

MTObjectTranslator::MTObjectTranslator()
{
}

MTObjectTranslator::~MTObjectTranslator()
{
}

bool MTObjectTranslator::CanHandleType(const char *mimeType)
{
	return false;
}

void MTObjectTranslator::Load(const char *mimeType, const void *data,size_t size)
{
}
