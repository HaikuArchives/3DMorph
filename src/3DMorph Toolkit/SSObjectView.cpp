/*
 * 3D Morph Toolkit
 *
 * Add-on pour voir les objets de l'écran de veille 3DMorph installé
 *
 * vue de configuration
 *
 * par Sylvain Tertois
 */

#include "SSObjectView.h"
#include "MyAddOn.h"
#include "resource3D.h"

#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>

SSObjectView::SSObjectView(MyAddOn *object)
: BBox(BRect(0,0,350,10),NULL,B_FOLLOW_LEFT_RIGHT)
{
	m_Object = object;
	
	SetLabel("3D Morph screen saver objects");
	if (m_Object == NULL)
		return;
	
	BPopUpMenu *l_Menu = new BPopUpMenu((const char*)NULL);
	unsigned int l_n = m_Object->m_Resource->NumObjects();
	for (unsigned int i=0; i<l_n; i++)
	{
		BString l_name;
		m_Object->m_Resource->GetName(i,l_name);
		
		BMessage *l_Message = new BMessage('ObjN');
		l_Message->AddInt32("num",i);
		
		BMenuItem *l_NewItem = new BMenuItem(l_name.String(),l_Message);
		
		l_Menu->AddItem(l_NewItem);
		if (i==0)
			l_NewItem->SetMarked(true);
	}
	
	m_MenuField = new BMenuField(BRect(10,16,200,36),NULL,"Object:",l_Menu);
	AddChild(m_MenuField);
	m_MenuField->ResizeToPreferred();
	m_MenuField->SetDivider(50);
	
	ResizeTo(350,m_MenuField->Frame().bottom+10);
}

SSObjectView::~SSObjectView()
{
}

void SSObjectView::AttachedToWindow()
{
	m_MenuField->Menu()->SetTargetForItems(this);
}

void SSObjectView::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
		case 'ObjN':
		{
			int32 l_n;
			if (message->FindInt32("num",&l_n) == B_OK)
			{
				m_Object->m_ObjectNum = l_n;
				m_Object->RequestUpdate();
			}
			break;
		}
		
		default:
			BBox::MessageReceived(message);
			break;
	}
}