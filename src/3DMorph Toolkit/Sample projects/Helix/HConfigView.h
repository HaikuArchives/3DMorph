/*
 * 3D Morph Toolkit
 *
 *
 * Helix Add-on
 *
 * This sample shows how to use the MTVector class, and how to make a
 * configuration view
 *
 * configuration view class
 */

#ifndef HCONFIGVIEW_H
#define HCONFIGVIEW_H


#include <Box.h>
class MyAddOn;
class BSlider;
class BColorControl;

class HConfigView : public BBox
{
public:
// construction and destruction
	HConfigView(MyAddOn*);
	virtual ~HConfigView();
	
// overrided functions
	virtual void AttachedToWindow();
		// called when the view is added to the Window. This function adjusts
		// the targets of the BControls
		
	virtual void MessageReceived(BMessage*);

private:
	MyAddOn *m_Object;	// pointer to the add-on object (given at construction)

	// controls
	BSlider *m_NumLines,*m_Diameter,*m_Length,*m_Twist;
	BColorControl *m_Color1, *m_Color2;	
};

#endif //HCONFIGVIEW_H
	