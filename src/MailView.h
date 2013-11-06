/*
*  MailView
*
* vue écrite en bleu souligné et qui lance une boite aux lettres avec
* le contenu de la vue en adresse mail
*/

#ifndef MAILVIEW_H
#define MAILVIEW_H

#include <StringView.h>


class MailView : public BStringView
{
public:
	MailView(BRect r,char *email);
	~MailView();
	
	void Draw(BRect);
	
	void MouseMoved(BPoint,uint32,const BMessage*);
	void MouseDown(BPoint);

private:
	BFont *font;
	char *email;
};

#endif //MAILVIEW_H