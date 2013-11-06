/* Librairie 3D
 * 
 * gestion d'un resource contenant des objets 3D créé par Resource Creator
 *
 * commencé le 28 décembre par Sylvain Tertois
 *
**/

#ifndef _RESOURCE3D_H
#define _RESOURCE3D_H

#include <StorageKit.h>
#include <kernel/OS.h>
#include <String.h>

class List3D;

class Resource3D 
{
public:
//	Resource3D();
// nouveau constructeur pour la version pour R4.5: on récupère la resource
// à partir de l'id d'image (kernel)
	Resource3D(image_id);
	~Resource3D();

	inline status_t InitCheck() { return initState; }	
	inline unsigned int NumObjects() { return numObjects; }
	inline unsigned int NumRealObjects() { return numRealObjects; }
	
	void LoadConfig(bool *config);	// charge la confioguration: pour savoir quels objets sont
	void SaveConfig(bool *config);	// actifs et lesquels ne le sont pas
	
	status_t GetData(unsigned int objectNum, List3D **points, rgb_color **colors,bool onlyEnabled = true);
/* les objets List3D et rgb_color sont alloués par GetData, il faut donc penser
   à les libérer plus tard.
   Si les pointeurs sont non nuls, les objets List3D et rgb_color sont d'abord détruits
   Si le booléen onlyEnabled est false, on peut accéder à tous les objets, même ceux qui ne sont pas actifs */
   void GetName(unsigned int,BString&);
   
   	// ATTENTION: les deux fonctions ci-dessous permettent de modifier la liste
   	// d'objets, mais après, la fonction GetData par défaut (avec onlyEnabled = true)
   	// ne marche plus... il faut faire un LoadConfig
   void RemoveObject(unsigned int num);	// enlève un objet de la liste
   bool AddObject(char *name,List3D &points,rgb_color *colors);
   			// ajoute un objet dans la liste et rend son numéro
   
   void Save();				// obligatoire pour écrire dans la resource les modifications faites 
   							// avec les deux fonctions ci-dessus
   
   void LoadCache();		// met les images activées en cache en mémoire
   void ClearCache();
   

private:
	BResources myResource;
	status_t initState;
	unsigned int numObjects;		// nombre d'objets activés
	unsigned int numRealObjects;	// nombre réel d'objets
	unsigned int *objectsNum;		// tableau de conversion vers les vrais numéros d'objets
	BMessage archive;				// l'archive qui contient les objets
	bool changed;					// true si la resource a été modifiée et nécessite une sauvegarde
	
	// gestion du cache d'images
	bool cache;
	List3D **cachePoints;
	rgb_color **cacheColors;
}; 

#endif
