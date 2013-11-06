/* Classe chaine de caractères
* Trap#1
*
* octobre 1998
*/

/* nom de la classe: MString
* construite à partir de: rien, char*, autre MString
* transtypage vers: char*
* opérateurs: Size, +, =, ==, !=, Find, Left, Right
*             [], Delete, Insert
*/

#ifndef _MYSTRING_H
#define _MYSTRING_H

class MString
{
public:
	MString();
	MString(const char *);
	MString(const MString&);
	MString(const char*,const char*); // construit la concaténation des 2 char*
	~MString();
	
	unsigned int Size() const;
	operator char*() const;
	friend MString& operator+ (const MString&, const char*);
	friend MString& operator+ (const char*, const MString&);
	friend MString& operator+ (const MString&,const MString&);
	MString& operator= (const char*);
	MString& operator= (const MString&);
	friend bool operator== (const MString&,const char*);
	friend bool operator== (const MString&,const MString&);
    char& operator[] (unsigned int i );
	
	unsigned int Find(char,unsigned int depart=0) const;
/* cherche un caractère dans une chaine. Si le caractère n'est pas trouvé,
* retourne la taille de la chaine */
	const MString& Left(unsigned int) const;
/* retourne une chaine composée des n premiers caractères*/
	const MString& Right(unsigned int) const;
/* retourne une chaine composée des caractères à partir du nième (exclus)*/
    void Delete(unsigned int);
/* enlève le caractère dont on donne l'indice */
    void Insert(char, unsigned int);
/* insère le caractère à la position donnée */	

private:
	char *pstring;
};


MString& operator+ (const MString&, const char*);
MString& operator+ (const char*, const MString&);
MString& operator+ (const MString&,const MString&);
bool operator== (const MString&,const char*);
bool operator== (const MString&,const MString&);

inline bool operator!= (const MString &s1,const char *s2) 
		{ return !(s1==s2); }
inline bool operator!= (const MString &s1, const MString &s2)
		{ return !(s1==s2); }
		
		
#endif
