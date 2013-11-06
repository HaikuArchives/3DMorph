/* Librairie 3D
 * 
 * gestion d'un resource contenant des objets 3D créé par Resource Creator
 *
 * commencé le 28 décembre par Sylvain Tertois
 *
 * version allégée (lecture uniquement) pour le toolkit (pas de config, ni
 * d'objets activés/désactivés, ni de cache)
 *
**/

#include "MTPoints.h"
#include "resource3D.h"

Resource3D::Resource3D(BFile *myFile)
{
	initState = B_ERROR;

	uint32 *nO;
	size_t size;
	
	if ( myFile->InitCheck() != B_OK )
		return;
	
	if ( myResource.SetTo(myFile) != B_OK )
		return;
	
	nO = (uint32*)myResource.LoadResource(B_UINT32_TYPE,(int32)0,&size);
	if (nO==NULL)
		return;
		
	numObjects = *nO;
	
	const char *flat = (char*)myResource.FindResource(B_RAW_TYPE,(int32)0,&size);
	archive.Unflatten(flat);
	
	initState = B_OK;
}

Resource3D::~Resource3D()
{
}

MTPoints *Resource3D::GetData(unsigned int objectNum)
{
	const float *coords;
	ssize_t sizeOfCoords;

	BString name;
	GetName(objectNum,name);
	
	if (archive.FindData(name.String(),B_FLOAT_TYPE,(const void**)&coords,
			&sizeOfCoords) != B_OK)
		return NULL;
	
	unsigned int numPoints = sizeOfCoords/(3*sizeof(float));

	ssize_t dummy;
	const rgb_color *colors;
	if (archive.FindData("colors",B_RGB_COLOR_TYPE,objectNum,
			(const void**)&colors,&dummy) != B_OK)
		return NULL;
	
	return new MTPoints(numPoints,coords,colors);
}

void Resource3D::GetName(unsigned int objectNum,BString &name)
{
	char *objName;
	type_code dummy;
	archive.GetInfo(B_FLOAT_TYPE,objectNum,&objName,&dummy);
	name = objName;
}