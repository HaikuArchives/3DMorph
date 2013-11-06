/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTImageObject class
 * Object Translator that loads image files, and creates 3D Morph objects
 * with them
 *
**/

#ifndef MTIMAGEOBJECT_H
#define MTIMAGEOBJECT_H

#include "MTObjectTrans.h"

class MTIO_ConfigView;
class BButton;
class BBitmap;

class MTImageObject : public MTObjectTranslator
{
public:
	MTImageObject();
	virtual ~MTImageObject();
	
	// overrided functions
	virtual bool CanHandleType(const char *mimeType);
	virtual void Load(const char *mimeType, const void *data, size_t size);
	
	virtual MTPoints *Update(float);
	virtual BView *CreateView();
	
	// calcule les dimensions de l'objet qu'on devra générer
	void CalcSize(unsigned int *x, unsigned int *y);

	// bitmaps avec les images
	BBitmap *m_ImageBitmap, *m_HeightFieldBitmap;

	// paramètres pour le nombre de points en X et Y
	typedef enum { X_32=0, X_IMAGE, X_OTHER } XPoints;
	typedef enum { Y_PROP=0, Y_X, Y_IMAGE, Y_OTHER } YPoints;
	XPoints m_xParams;
	YPoints m_yParams;
	
	// amplitude du 'height field'
	float m_HeightAmplitude;
	
	// options
	bool m_PropObject, m_IgnoreBlackPoints;
	typedef enum { O_HORIZ = 0, O_VERT, O_ZIGZAG, O_RAND } Order;
	Order m_Order;
	typedef enum { P_XY = 0, P_XZ, P_YZ } Plane;
	Plane m_Plane;
	
	// autres trucs
private:
	MTIO_ConfigView *m_ConfigView;
};

#endif //MTIMAGEOBJECT_H	