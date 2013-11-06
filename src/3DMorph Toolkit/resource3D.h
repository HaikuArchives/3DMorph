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

#ifndef _RESOURCE3D_H
#define _RESOURCE3D_H

#include <StorageKit.h>
#include <String.h>

class MTPoints;

class Resource3D 
{
public:
// nouveau constructeur pour le toolkit. On s'appuie sur l'objet BFile représentant
// 3DMorph
	Resource3D(BFile *);	// l'objet BFile peut être supprimé après l'appel
	~Resource3D();

	inline status_t InitCheck() { return initState; }	
	inline unsigned int NumObjects() { return numObjects; }
	
	MTPoints *GetData(unsigned int objectNum);
	void GetName(unsigned int,BString&);
   

private:
	BResources myResource;
	status_t initState;
	unsigned int numObjects;		// nombre d'objets activés
	BMessage archive;				// l'archive qui contient les objets
}; 

#endif
