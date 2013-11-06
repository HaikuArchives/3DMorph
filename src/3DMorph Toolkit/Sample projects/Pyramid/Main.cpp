/*
 * 3D Morph Toolkit
 *
 * this file is falcutative
 * it just makes add-on development easier.
 * There is here a main() function, so that the add-on can be started just
 * like any other application. It will launch the toolkit with the correct 
 * add-on.
 *
 * You don't need to change this file
 *
 * If you don't want this option in your add-on, remove this file from the
 * project, and in the settings, go to 'x86 ELF Project', and select
 * 'shared library' instead of 'Application'
 *
 * by Sylvain Tertois
 */

#include <Application.h>
#include <Roster.h>
#include <Path.h>
#include <image.h>
#include <Alert.h>

#include "MTAddOn.h"

class MyApp : public BApplication
{
public:
	MyApp();
	
	virtual void ReadyToRun();
};

// main function. Creates the BApplication object and runs it
int main()
{
	MyApp l_myApplication;
	l_myApplication.Run();
	
	return 0;
}

// application constructor
MyApp::MyApp()
	: BApplication("application/x-vnd.STertois-3dMorph.Toolkit")
{
}

// function called when the application is initialized
// locates the 3DMorph toolkit and transfer control to him
void MyApp::ReadyToRun()
{
	// finds the toolkit path
	entry_ref l_ToolkitRef;
	if (be_roster->FindApp("application/x-vnd.STertois-3dMorph.Toolkit",
		&l_ToolkitRef) == B_OK)
	{
		BPath l_ToolkitPath(&l_ToolkitRef);
		
		// load the toolkit, as an add-on
		image_id l_ToolkitID = load_add_on(l_ToolkitPath.Path());
		if (l_ToolkitID >= 0)
		{
			// find the LaunchWithAddOn function
			void (*l_Function)(MTAddOnType,MTObject*);
			if (get_image_symbol(l_ToolkitID,"LaunchWithAddOn",
				B_SYMBOL_TYPE_TEXT,(void**)&l_Function) == B_OK)
			{
				// launch the toolkit
				(*l_Function)(addOnType,InstantiateObject());
				
				return;
			}
			else
				(new BAlert("3DM Add-on","Can't launch the toolkit","Cancel",
					NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
		}
		else
			(new BAlert("3DM Add-on","Can't load the toolkit","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
	}
	else
		(new BAlert("3DM Add-on","Can't find the toolkit","Cancel",
			NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
	
	// an error occured. We must close the application
	PostMessage(B_QUIT_REQUESTED);
}
			