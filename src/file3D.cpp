/* écran de veille 3dMorph */

/* Gestion des fichiers texte contenant un objet 3d */

/* commencé le 20 décembre 1998 par Sylvain Tertois */

#include "MyString.h"
#include "TextFile.h"
#include "list3D.h"
#include "file3D.h"



File3D::File3D(BEntry &file_entry)
{
	file = new BFile(&file_entry,B_READ_ONLY);
	
	if ( (init_status = file->InitCheck()) == B_OK )
	{
// On ne continue que si le fichier a été correctement ouvert
		MString line;
		numPoints = 0;
// Utilisation du fichier comme texte
		TextFile text(*file);
		
		while ((numPoints == 0) && !text.Eof())
		{
// chargement des lignes une à une
			text.ReadString(line);
			if ( line.Left(2) == "#N" )
// La ligne qui commence par #N donne le nombre de points: c'est ce qu'on cherche
				if ( sscanf( line.Right(2), " %d", &numPoints ) != 1 )
					numPoints = 0;
		}
// Si numPoints est toujours égal à 0, c'est qu'il y a un problème
		if (numPoints == 0)
			init_status = B_ERROR;
	}
}

File3D::~File3D()
{
	delete file;
}

status_t File3D::LoadInto(List3D &points, rgb_color *colors)
{
	status_t returnStatus = B_OK;
	TextFile text(*file);
	MString line;
	
// boucle principale (éxécutée n fois, tant qu'il n'y a pas d'erreur
	for ( unsigned int i=0; (returnStatus==B_OK) && (i<numPoints); i++)
	{
//		printf("ligne %d...\n",i);
		line="";
// recherche d'une ligne valide (tant qu'il n'y a pas d'erreur)
		while ((returnStatus==B_OK) && !(line.Left(2) == "#P" ))
		{
			if ( text.Eof() )
				returnStatus = B_ERROR;
			else
				text.ReadString(line);
		}
// récupération des données
		float x,y,z,r,g,b;
		if ( sscanf(line.Right(2)," %f %f %f %f %f %f",&x,&y,&z,&r,&g,&b) != 6)
			returnStatus = B_ERROR;
		else
		{
// écriture dans les tableaux
			points.PutPoint(i,x,y,z);
			colors[i].red=(uint8)(r*255);
			colors[i].green=(uint8)(g*255);
			colors[i].blue=(uint8)(b*255);
		}
	}
	return(returnStatus);
} 
