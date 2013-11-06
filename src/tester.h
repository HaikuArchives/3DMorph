/* testeur d'un écran de veille Blanket */

/* commencé le 15 décembre 1998 par Sylvain Tertois */


#ifndef _TESTER_H
#define _TESTER_H


class TesterApp : public BApplication
{
	public:
		TesterApp();
		~TesterApp();

	private:
		CMorphThread *saver;
};

class TesterWin : public BWindow
{
	public:
		TesterWin(BRect &r,char *name,window_type type,int flags);
		bool QuitRequested();
};


#endif
