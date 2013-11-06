/* créateur de resources pour mon écran de veille
 *
 * commencé le 28 décembre 1998 par Sylvain Tertois
**/

#include "list3D.h"
#include "file3D.h"
#include "resource creator.h"
#include <stdlib.h>

// répertoire qui contient les objets 3d
const char *obj3dpath="3D Objects";
// nom du fichier resource
const char *resource="3DObjects.rsrc";
// nom des éléments de la resource
const char *rNumObjects="Number of Objects";
const char *rObjects="Objects";
// les autres éléments ont le nom de l'objet associé

main()
{
	ResourceCreator myApplication;
	myApplication.Run();
}

ResourceCreator::ResourceCreator() : BApplication("application/x-Trap1-3dmorph.rsrc")
{
/* construction de execDir */
	app_info myInfo;
	GetAppInfo(&myInfo);
	
	BEntry execFileEntry(&(myInfo.ref));
	
	if (execFileEntry.GetParent(&execDir)!=B_OK)
		printf("erreur à la création du répertoire execDir\n");
	else
	{
		printf("Chargement des objets\n");
		LoadObjects();
		printf("Sauvegarde du fichier resource\n");
		SaveResource();
		printf("Terminé!\n");
		PostMessage(B_QUIT_REQUESTED);
	}
}

ResourceCreator::~ResourceCreator()
{
}

void ResourceCreator::LoadObjects()
{
	BDirectory objectsDirectory(&execDir,obj3dpath);
	
	if (objectsDirectory.InitCheck() != B_OK)
	{
		printf("Erreur: répoertoire objet non trouvé\n");
		return;
	}
	
	BEntry objectEntry;
	
	while (objectsDirectory.GetNextEntry(&objectEntry)==B_OK)
	{
		char fname[B_FILE_NAME_LENGTH];
		objectEntry.GetName(fname);
		printf("Lecture de %s...",fname);

		File3D object(objectEntry);
		
		if (object.InitCheck() != B_OK)
			printf("Erreur à l'ouverture du fichier\n");
		else
		{
			int n = object.NumPoints();
			List3D *points = new List3D(n);
			rgb_color *colors = new rgb_color[n];
			
			if (object.LoadInto(*points,colors) != B_OK)
				printf("Erreur au chargement\n");
			else
			{
				objects.AddItem(new BString(fname));
				objects.AddItem((void*)points);
				objects.AddItem((void*)colors);
				printf("Ok\n");
			}
		}
	}
}

void ResourceCreator::SaveResource()
{
	BFile myFile;
	
	if (execDir.CreateFile(resource,&myFile) != B_OK )
	{
		printf("Erreur à la création du fichier\n");
		return;
	}
	
	BResources myResource;
	if (myResource.SetTo(&myFile,true) != B_OK)
	{
		printf("Erreur à la création du resource\n");
		return;
	}
	
	uint32 nO = objects.CountItems()/3;
	
	myResource.AddResource(B_UINT32_TYPE,0,&nO,sizeof(uint32),rNumObjects);
	
	BMessage archive('ARCV');
	
	for (unsigned int i=0; i<nO; i++)
	{
		BString *name = (BString*)objects.ItemAt(i*3);
		List3D *points = (List3D*)objects.ItemAt(i*3+1);
		rgb_color *colors = (rgb_color*)objects.ItemAt(i*3+2);
		int nP = points->NumPoints();
		
		archive.AddData(name->String(),B_FLOAT_TYPE,points->points,nP*sizeof(float)*3);
		archive.AddData("colors",B_RGB_COLOR_TYPE,colors,nP*sizeof(rgb_color),false);
		
		delete points;
		delete colors;
		delete name;
		printf("Objet n°%d écrit.\n",i);
	}
	BMallocIO buffer;
	archive.Flatten(&buffer);
	myResource.AddResource(B_RAW_TYPE,0,buffer.Buffer(),buffer.BufferLength(),rObjects);
}
