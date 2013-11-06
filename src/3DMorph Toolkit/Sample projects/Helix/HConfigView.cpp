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

#include "HConfigView.h"
#include "MyAddOn.h"

#include <Slider.h>
#include <Window.h>
#include <ColorControl.h>

// message type used to say update is needed
static const int32 c_UpdateMessage = 'Updt';

// construction and destruction
HConfigView::HConfigView(MyAddOn *object)
: BBox(BRect(0,0,350,10),NULL,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP)
{
	// copy the pointer to the object
	m_Object = object;
	
	// a few variables which will be helpful to place the different controls
	static const float c_margin = 10;		// margin between the objects
	static const float c_sliderWidth = 200;	// width of a slider
	float l_top = c_margin+6;
	float l_bottom,l_right;
	BRect l_Rect(c_margin,l_top,c_margin+c_sliderWidth,l_top+10);
		// rectangle used to place the controls
	
	// Controls
	/// number of lines
	//// we use the previous defined rectangle to create a slider, which can
	//// go between 1 and 32
	m_NumLines = new BSlider(l_Rect,NULL,"Number of lines",NULL,1,32);
	
	//// now we can setup labels and hashmarks
	m_NumLines->SetLimitLabels("1","32");
	m_NumLines->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_NumLines->SetHashMarkCount(16);
	
	//// set the initial value
	m_NumLines->SetValue(1);
	
	//// the update message will be sent at each modification of the slider
	m_NumLines->SetModificationMessage(new BMessage(c_UpdateMessage));
	
	//// now we can resize the slider to its preferred size, and add it to the view
	m_NumLines->ResizeToPreferred();
	AddChild(m_NumLines);
	
	//// we get back its bottom coordinate, to update the rectangle for the next
	//// slider
	l_Rect.top = m_NumLines->Frame().bottom+c_margin;
	l_Rect.bottom = l_Rect.top+10;
	
	//// and we go on for the next control
	
	/// twist
	m_Twist = new BSlider(l_Rect,NULL,"Twisting",NULL,1,1000);
	m_Twist->SetLimitLabels("straight","very twisted");
	m_Twist->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_Twist->SetHashMarkCount(16);
	m_Twist->SetPosition(0);
	m_Twist->SetModificationMessage(new BMessage(c_UpdateMessage));
	m_Twist->ResizeToPreferred();
	AddChild(m_Twist);
	
	l_Rect.top = m_Twist->Frame().bottom+c_margin;
	l_Rect.bottom = l_Rect.top+10;
	
	/// first color
	m_Color1 = new BColorControl(l_Rect.LeftTop(),B_CELLS_16x16,2,NULL,
		new BMessage(c_UpdateMessage));
	m_Color1->SetValue(0xffffff00);	// white
	AddChild(m_Color1);
	
	/// calculate the bottom of the view
	l_bottom = m_Color1->Frame().bottom + c_margin;
	
	//// the next slider won't be put underneath, but on the right
	l_Rect.OffsetTo(l_Rect.right+c_margin,l_top);
	
	/// Diameter
	m_Diameter = new BSlider(l_Rect,NULL,"Diameter",NULL,1,1000);
	m_Diameter->SetLimitLabels("0","1");
	m_Diameter->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_Diameter->SetHashMarkCount(16);
	m_Diameter->SetPosition(0.5);
	m_Diameter->SetModificationMessage(new BMessage(c_UpdateMessage));
	m_Diameter->ResizeToPreferred();
	AddChild(m_Diameter);
	
	l_Rect.top = m_Diameter->Frame().bottom+c_margin;
	l_Rect.bottom = l_Rect.top+10;
	
	/// Length
	m_Length = new BSlider(l_Rect,NULL,"Length",NULL,1,1000);
	m_Length->SetLimitLabels("0","1");
	m_Length->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_Length->SetHashMarkCount(16);
	m_Length->SetPosition(1);
	m_Length->SetModificationMessage(new BMessage(c_UpdateMessage));
	m_Length->ResizeToPreferred();
	AddChild(m_Length);
	
	l_Rect.top = m_Length->Frame().bottom+c_margin;
	l_Rect.bottom = l_Rect.top+10;

	/// calculate the width of the view
	l_right = m_Length->Frame().right+c_margin;
		
	/// second color
	m_Color2 = new BColorControl(l_Rect.LeftTop(),B_CELLS_16x16,2,NULL,
		new BMessage(c_UpdateMessage));
	m_Color2->SetValue(0xffffff00);	// white
	AddChild(m_Color2);
	
	/// recalculate the width of the view
	if (m_Color2->Frame().right+c_margin>l_right)
		l_right = m_Color2->Frame().right+c_margin;
	
	ResizeTo(l_right,l_bottom);
}

HConfigView::~HConfigView()
{
}

void HConfigView::AttachedToWindow()
{
	// set the controls targets
	m_NumLines->SetTarget(this);
	m_Diameter->SetTarget(this);
	m_Length->SetTarget(this);
	m_Twist->SetTarget(this);
	
	m_Color1->SetTarget(this);
	m_Color2->SetTarget(this);
	
	// request an update
	Window()->PostMessage(c_UpdateMessage,this);
		// the message is sent to this view
}

void HConfigView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case c_UpdateMessage:
			// update the object parameters
			m_Object->m_NumLines = (unsigned int)m_NumLines->Value();
			m_Object->m_Diameter = m_Diameter->Position();
			m_Object->m_Length = m_Length->Position();
			m_Object->m_Twist = m_Twist->Position();
			
			m_Object->m_Color1 = m_Color1->ValueAsColor();
			m_Object->m_Color2 = m_Color2->ValueAsColor();
			
			// and request an update
			m_Object->RequestUpdate();
		break;
		
		default:
			BBox::MessageReceived(message);
			break;
	}
}
