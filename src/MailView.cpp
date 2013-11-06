/*
*  MailView
*
* vue écrite en bleu souligné et qui lance une boite aux lettres avec
* le contenu de la vue en adresse mail
*/

#include "MailView.h"
#include <Font.h>
#include <Application.h>
#include <Roster.h>
#include <String.h>

static const char mailMouse[] =
{	16,1,7,7,
	0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
	0x80,0x01,0x80,0x15,0xbf,0x89,0x80,0x15,
	0xbf,0x81,0x80,0x01,0xbf,0x81,0x80,0x01,
	0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00
};
	
MailView::MailView(BRect r,char *e)
	: BStringView(r,NULL,e,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW)
{
	email=e;
	
	SetHighColor(0,0,255,0);
	font = new BFont(be_plain_font);
	font->SetFace(B_UNDERSCORE_FACE);
	SetFont(font);
	
	r.right = r.left+font->StringWidth(e);
	ResizeTo(r.Width(),r.Height());
}

MailView::~MailView()
{
	delete font;
}

void MailView::Draw(BRect r)
{
	BStringView::Draw(r);

	font_height h;
	font->GetHeight(&h);
	
	r = Bounds();
	r.bottom -= h.descent;
	r.right = r.left + font->StringWidth(email);
	StrokeLine(r.LeftBottom(),r.RightBottom());
}

void MailView::MouseMoved(BPoint p, uint32 transit, const BMessage *bla)
{
	switch(transit)
	{
		case B_ENTERED_VIEW:
			be_app->SetCursor(mailMouse);
			break;
		
		case B_EXITED_VIEW:
			be_app->SetCursor(B_HAND_CURSOR);
			break;
		
		default:
			break;
	}
}

void MailView::MouseDown(BPoint p)
{
	BString m = "mailto:";
	m += email;
	
	char *argv[2]= { "app", (char*)m.String() };
	
	be_roster->Launch("text/x-email",2,argv);
}
