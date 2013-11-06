/* Librairie 3D
 * 
 * gestion d'un resource contenant des objets 3D créé par Resource Creator
 *
 * commencé le 28 décembre par Sylvain Tertois
 *
**/

#include "list3D.h"
#include "resource3D.h"

Resource3D::Resource3D(image_id image)
{
	initState = B_ERROR;

	uint32 *nO;
	size_t size;
	
	image_info myInfo;
	
	if (get_image_info(image,&myInfo) != B_OK)
		return;
	
	BFile myFile(myInfo.name,B_READ_WRITE);
	
	if ( myFile.InitCheck() != B_OK )
		return;
	
	if ( myResource.SetTo(&myFile) != B_OK )
		return;
	
	nO = (uint32*)myResource.LoadResource(B_UINT32_TYPE,(int32)0,&size);
	if (nO==NULL)
		return;
		
	numRealObjects=numObjects = *nO;
	objectsNum = NULL;
	
	const char *flat = (char*)myResource.FindResource(B_RAW_TYPE,(int32)0,&size);
	archive.Unflatten(flat);
	
	changed = false;
	
	initState = B_OK;
	
	cache = false;
}

Resource3D::~Resource3D()
{
	if (cache)
		ClearCache();
}

status_t Resource3D::GetData(unsigned int objectNum,List3D **points, rgb_color **colors,bool onlyEnabled)
{
	// si l'image est dans le cache, on la prend là
	if (onlyEnabled && cache)
	{
		delete *points;
		*points = new List3D(*(cachePoints[objectNum]));
		
		delete[] *colors;
		*colors = new rgb_color[(*points)->NumPoints()];
		
		for (unsigned int i=0; i<(*points)->NumPoints(); i++)
			(*colors)[i] = cacheColors[objectNum][i];
		
		return B_OK;
	}

	// on prend l'objet réel associé au numéro (si on ne veut qu'un objet activ, ce qui est le cas en général)
	if (onlyEnabled && (objectsNum != NULL))
		objectNum = objectsNum[objectNum];
		
	delete *points;
	delete[] *colors;

	*points = NULL;
	*colors = NULL;
	
	float *coords;
	ssize_t sizeOfCoords;

	BString name;
	GetName(objectNum,name);
	
	const float *mcr;
	if (archive.FindData(name.String(),B_FLOAT_TYPE,(const void**)&mcr,&sizeOfCoords) != B_OK)
		return (B_ERROR);
	
	unsigned int numPoints = sizeOfCoords/(3*sizeof(float));

	coords = new float[numPoints*3];
	for (unsigned int i=0; i<numPoints*3; i++)
		coords[i] = mcr[i];
	*points = new List3D(numPoints,coords);
	
	ssize_t dummy;
	const rgb_color *mcl;
	if (archive.FindData("colors",B_RGB_COLOR_TYPE,objectNum,(const void**)&mcl,&dummy) != B_OK)
		return (B_ERROR);
	
	*colors = new rgb_color[numPoints];
	for (unsigned int i=0; i<numPoints;i++)
		(*colors)[i] = mcl[i];
	
	return (B_OK);
}

void Resource3D::GetName(unsigned int objectNum,BString &name)
{
	char *objName;
	type_code dummy;
	archive.GetInfo(B_FLOAT_TYPE,objectNum,&objName,&dummy);
	name = objName;
}


void Resource3D::LoadConfig(bool *config)
{
	numObjects = 0;
	for (unsigned int i=0; i<numRealObjects; i++)
		if(config[i])
			numObjects++;
	
	delete objectsNum;
	objectsNum = new unsigned int[numObjects];
	
	unsigned int j=0;
	for (unsigned int i=0; i<numRealObjects; i++)
		if (config[i])
			objectsNum[j++] = i;
}

void Resource3D::SaveConfig(bool *config)
{
	if (objectsNum == NULL)
		for (unsigned int i=0; i<numRealObjects; i++)
			config[i] = true;
	else
	{
		unsigned int j=0;
		for (unsigned int i=0; i<numRealObjects; i++)
			if (objectsNum[j] == i)
			{
				config[i] = true;
				j++;
			}
			else
				config[i] = false;
	}
}

void Resource3D::RemoveObject(unsigned int num)
// ATTENTION!! après avoir appelé cette fonction, les numéros d'objets 'normaux' ne
// sont plus valides... il faut faire un LoadConfig ensuite pour pouvoir à nouveau
// appeler GetData avec un onlyEnabled à true
{
	BString name;
	GetName(num,name);
	
	archive.RemoveData(name.String());
	archive.RemoveData("colors",num);
	numRealObjects--;
	
	changed = true;
	if (cache)
		ClearCache();
}

bool Resource3D::AddObject(char *name,List3D &points,rgb_color *colors)
{
	// si le nom est déjà utilisé, il faut prendre autre chose
	if (archive.HasData(name,B_FLOAT_TYPE))
		return false;
		
	archive.AddData(name,B_FLOAT_TYPE,points.points,points.NumPoints()*3*sizeof(float));
	archive.AddData("colors",B_RGB_COLOR_TYPE,colors,points.NumPoints()*sizeof(rgb_color),false);
	
	changed = true;
	if (cache)
		ClearCache();
	
	numRealObjects++;
	
	return true;
}

void Resource3D::Save()
{
	if (changed)
	{
		myResource.RemoveResource(B_RAW_TYPE,0);
		myResource.RemoveResource(B_UINT32_TYPE,0);
		
		uint32 n0 = numRealObjects;
		
		myResource.AddResource(B_UINT32_TYPE,0,&n0,sizeof(uint32),"Number of Objects");
		
		BMallocIO buffer;
		archive.Flatten(&buffer);
		myResource.AddResource(B_RAW_TYPE,0,buffer.Buffer(),buffer.BufferLength(),"Objects");
	
		myResource.Sync();
		
		changed = false;
	}
}

void Resource3D::ClearCache()
{
	if (!cache)
		return;
		
	cache = false;

	delete[] cachePoints;
	delete[] cacheColors;
}

typedef List3D* ptrList3D;
typedef rgb_color* ptrRgb_color;

void Resource3D::LoadCache()
{
	if (cache)
		ClearCache();
	
	cachePoints = new ptrList3D[numObjects];
	cacheColors = new ptrRgb_color[numObjects];
	
	for (unsigned int i=0; i<numObjects; i++)
	{
		cachePoints[i]=NULL;
		cacheColors[i]=NULL;
		
		GetData(i,&(cachePoints[i]),&(cacheColors[i]));
	}
	
	cache = true;		
}