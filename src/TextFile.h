/* Classe primitive de fichiers texte
* Trap#1
*
* la particularité de cette classe est qu'elle peut gérer aussi bien des textes
* au format Be (Unix) que Dos
*
* Cette classe est construite à partir d'un BFile et a deux méthodes:
* MString ReadString()
* WriteString(MString)
* 
* et une variable: iDosMode
*
* iDosMode peut prendre une des valeurs:
*  DM_UNKNOWN (pour l'écriture, DM_UNKNOWN a le même résultat que DM_BE)
*  DM_BE
*  DM_DOS
*  DM_BOTH (pour l'écriture, DM_BOTH a le même résultat que DM_BE)
*
* iDosMode est égal à DM_UNKNOWN tant qu'aucune lecture n'a été effectuée sur
* le fichier ou que l'utilisateur ne l'ait directement modifiée
*
* le destructeur ne détruit pas le fichier
*
*
* !!! ATTENTION !!! la taille d'une ligne ne doit pas être supérieure à 
* TF_BUFFER_SIZE. sinon la fin de la ligne sera perdue à la lecture
*
*/

#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <StorageKit.h>

typedef enum { DM_UNKNOWN, DM_BE, DM_DOS, DM_BOTH } DosMode;
#define TF_BUFFER_SIZE 1024

class TextFile
{
public:

    TextFile(const BFile&); 
	TextFile(const TextFile&);
    
    void ReadString(MString&);
    int WriteString(MString);
/* retourne le nombre d'octets écrits ou un entier négatif si erreur */
    bool Eof();
    
    DosMode iDosMode;
    
private:
	BFile file;
	char buffer[TF_BUFFER_SIZE];
};

#endif