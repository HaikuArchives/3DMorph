/* écran de veille 3dMorph */

/* Gestion des fichiers texte contenant un objet 3d */

/* commencé le 20 décembre 1998 par Sylvain Tertois */

/* format du fichier texte:
* toutes les lignes utiles commencent par un # et une lettre. Le reste de la ligne
  dépend de la lettre
#Nentier          : donne le nombre de points (ex: #N512)
#Px y z r g b     : définit un point (ex: #P0.0 0.5 1.4 1.0 0.5 0.5)
                    toutes les grandeurs sont flottantes comprises entre 0 et 1
*/

#ifndef _FILE3D_H
#define _FILE3D_H

class File3D
{
public:
	File3D(BEntry &);
	~File3D();
		
	inline status_t InitCheck()	// retourne B_OK ou une erreur si l'initialisation s'est mal passée
			{ return init_status; }
	inline unsigned int NumPoints()	// donne le nombre de points de la figure
			{ return numPoints; }
	status_t LoadInto(List3D&, rgb_color*); // charge l'objet 
			/* B_OK si pas d'erreur
			 * les listes doivent être initialisées avec la bonne taille */

private:
	BFile		*file;
	status_t 	init_status;
	unsigned int numPoints;
};

#endif