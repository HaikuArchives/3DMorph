/*
 * 3D Morph Toolkit
 *
 * Add-on pour voir les objets de l'écran de veille 3DMorph installé
 *
 * vue de configuration
 *
 * par Sylvain Tertois
 */

#ifndef SSOBJECTVIEW_H
#define SSOBJECTVIEW_H

#include <Box.h>

class BMenuField;
class MyAddOn;

class SSObjectView : public BBox
{
public:
	SSObjectView(MyAddOn*);
	virtual ~SSObjectView();
	
	virtual void AttachedToWindow();
	
	virtual void MessageReceived(BMessage*);
	
private:
	MyAddOn *m_Object;
	BMenuField *m_MenuField;
};

#endif //SSOBJECTVIEW_H