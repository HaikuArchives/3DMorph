/* Classe de fichiers texte
* Trap#1
* commencée le 30 novembre 1998
*/

#include "MyString.h"
#include "TextFile.h"
#include <StorageKit.h>

TextFile::TextFile(const BFile &f)
{
	file=f;
	iDosMode=DM_UNKNOWN;
}

TextFile::TextFile(const TextFile &tf)
{
	file=tf.file;
	iDosMode=tf.iDosMode;
}

void TextFile::ReadString(MString &ret)
{
	bool cont = true,dos_flag = false;
	ssize_t bufferSize;

	if ( (bufferSize=file.Read(buffer,TF_BUFFER_SIZE)) < 1)
	{
		ret="";
		return;
	}

	char *strBuffer = new char[TF_BUFFER_SIZE+1];
	char *strPtr = strBuffer;
	
	int i;
	for (i=0; i<bufferSize && cont; i++)
	{
		char ch = buffer[i];
		if (ch == 0x0a)
			cont=false;
		else
			if (ch == 0x0d )
				dos_flag = false;
			else
				*strPtr++=ch;
	}
	*strPtr=0;
	ret = strBuffer;
	file.Seek(i-bufferSize,1);
	
	if (iDosMode == DM_UNKNOWN )
		if ( dos_flag )
			iDosMode = DM_DOS;
		else
			iDosMode = DM_BE;
	if ( (iDosMode == DM_BE && dos_flag) || (iDosMode == DM_DOS && !dos_flag) )
		iDosMode = DM_BOTH;
	
	delete[] strBuffer;	
}

int TextFile::WriteString(MString str)
{
	str=str+"\n";
	if (iDosMode==DM_DOS)
	{
// recherche de tous les 0x0a et ajoute un 0x0d devant
		unsigned int i,s=str.Size();
		
		i=str.Find(0x0a);
		while(i<s)
		{
			str.Insert(0x0d,i);
			s=str.Size();
			i=str.Find(0x0a,i+2);
		}
	}
	return (file.Write((char*)str,str.Size()));
}

bool TextFile::Eof()
{
	off_t s;
	file.GetSize(&s);
	return(file.Position()==s);
}

