/* testeur d'un écran de veille Blanket */

/* commencé le 15 décembre 1998 par Sylvain Tertois */

#include "morph.h"
#include "tester.h"


int main()
{
	TesterApp myApplication;
	
	myApplication.Run();
	
	return(0);
}

TesterApp::TesterApp() : BApplication("application/x-Trap1-SaverTest")
{
	BRect r(0,0,600,600);
	BRect rWin(r);
	
	rWin.OffsetBy(100,100);
	TesterWin *myWin = new TesterWin(rWin,"Test d'écran de veille",B_TITLED_WINDOW,B_NOT_RESIZABLE|B_NOT_ZOOMABLE);
	BView *myView = new BView(r,NULL,B_FOLLOW_ALL,B_WILL_DRAW);
	
	myWin->Show();
	myWin->Lock();
	myWin->AddChild(myView);
	myView->SetViewColor(0,0,0);
	myView->SetLowColor(0,0,0);
	myView->Invalidate();
	myWin->Unlock();
	
	myWin->UpdateIfNeeded();
	
	module_start_saving(myView);
}

TesterApp::~TesterApp()
{
	module_stop_saving();
}

TesterWin::TesterWin(BRect &r, char *name, window_type type, int flags)
	: BWindow(r,name,type,flags)
{
}

bool TesterWin::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}