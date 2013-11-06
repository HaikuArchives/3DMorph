/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTTextObject class
 * Object Translator that loads text files, with the coordinates and colors
 *
**/

#ifndef MTTEXTOBJECT_H
#define MTTEXTOBJECT_H

#include "MTObjectTrans.h"

class MTTO_ConfigView;
class BButton;
struct MTTO_BufferInfo;	// structure utilisée pour lire le fichier texte
class BString;

class MTTextObject : public MTObjectTranslator
{
public:
	MTTextObject();
	virtual ~MTTextObject();
	
	// overrided functions
	virtual bool CanHandleType(const char *mimeType);
	virtual void Load(const char *mimeType, const void *data, size_t size);
	
	virtual MTPoints *Update(float);
	virtual BView *CreateView();

private:
	bool ReadLine(BString &line, struct MTTO_BufferInfo*);
	
	MTPoints *m_Points;
	MTTO_ConfigView *m_ConfigView;
};

#endif //MTTEXTOBJECT_H