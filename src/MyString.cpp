/* Classe de chaine de caractères
* Trap#1
*
* octobre 1998
*/

#include <SupportDefs.h>
#include <string.h>
#include "MyString.h"

MString::MString()
{
	pstring = (char*)B_EMPTY_STRING;
}

MString::MString(const char *s)
{
	pstring = new char[ strlen(s)+1 ];
	strcpy(pstring,s);
}

MString::MString(const MString &s)
{
	pstring = new char [ s.Size()+1 ];
	strcpy(pstring,s.pstring);
}

MString::~MString()
{
	if (pstring!=B_EMPTY_STRING)
		delete pstring;
}

unsigned int MString::Size() const
{
	return (strlen(pstring) );
}

MString::operator char*() const
{
	return(pstring);
}

MString::MString (const char *s1, const char *s2)
{
	pstring = new char [ strlen(s1) + strlen(s2) + 1 ];
	strcpy (pstring, s1);
	strcat (pstring, s2);
}

MString& operator + (const MString &s1, const char *s2)
{
	MString *dest;
	
	dest = new MString(s1.pstring,s2);
	return(*dest);
}

MString& operator + (const char *s1, const MString &s2)
{
	MString *dest;
	
	dest = new MString(s1,s2.pstring);
	return(*dest);
}

MString& operator + (const MString &s1, const MString &s2)
{
	MString *dest;
	
	dest = new MString(s1.pstring,s2.pstring);
	return(*dest);
}

MString& MString::operator = (const char *s)
{
	if (pstring != B_EMPTY_STRING)
		delete pstring;
	pstring = new char[ strlen(s) +1 ];
	
	strcpy(pstring,s);
	return (*this);
}

MString& MString::operator = (const MString &s)
{
	if (pstring != B_EMPTY_STRING)
		delete pstring;
	pstring = new char[ s.Size() +1 ];
	
	strcpy(pstring,s.pstring);
	return (*this);
}

bool operator == (const MString &s1,const MString &s2)
{
	return (strcmp(s1.pstring,s2.pstring) == 0 );
}

bool operator == (const MString &s1,const char *s2)
{
	return (strcmp(s1.pstring,s2) == 0 );
}

char& MString::operator[] (unsigned int i)
{
	return(pstring[i]);
}

unsigned int MString::Find(char c, unsigned int i) const
{
	unsigned int s = Size();
	
	if (s==0)
		return 0;
	for (; i<s; i++)
		if (pstring[i]==c)
			return (i);
	return(s);
}

const MString &MString::Left(unsigned int n) const
{
unsigned int s = Size();

	if ( n >= s )
		n=s;
	
	MString *newstring = new MString;
	
	newstring->pstring = new char[n+1];
	strncpy(newstring->pstring,pstring,n);
	newstring->pstring[n]='\0';
	return (*newstring);
}

const MString &MString::Right(unsigned int n) const
{
unsigned int s = Size();

	if ( n >= s )
		n=s;
		
	MString *newstring = new MString;
	
	newstring->pstring = new char[s-n+1];
	strcpy(newstring->pstring,pstring+n);
	return(*newstring);
}

void MString::Delete(unsigned int pos)
{
	unsigned int i,s=Size();
	
	if (pstring==B_EMPTY_STRING || pos>=s)
		return;
	
	char *newstring = new char[s];
	
	if (pos>0)
	{
		for (i=0;i<pos;i++)
			newstring[i]=pstring[i];
	}
	for (i=pos;i<=s;i++)
		newstring[i]=pstring[i+1];

	delete pstring;
	pstring=newstring;
}

void MString::Insert(char c,unsigned int pos)
{
	unsigned int i,s=Size();
	
	if (pstring==B_EMPTY_STRING || pos>s)
		return;
		
	char *newstring = new char[s+2];
	
	if (pos>0)
	{
		for (i=0;i<pos;i++)
			newstring[i]=pstring[i];
	}
	newstring[pos]=c;
	for (i=pos;i<=s;i++)
		newstring[i+1]=pstring[i];
		
	delete pstring;
	pstring=newstring;
}	