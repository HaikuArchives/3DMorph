/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTObject class, used in add-ons
 *
 * default functions
 *
**/

#include "MTPoints.h"
#include "MTObject.h"

MTObject::MTObject()
{
}

MTObject::~MTObject()
{
}

MTPoints *MTObject::Update(float)
{
	return new MTPoints;
}

BView *MTObject::CreateView()
{
	return NULL;
}