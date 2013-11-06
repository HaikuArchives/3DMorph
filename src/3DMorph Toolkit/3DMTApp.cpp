/*
 * 3D Morph Toolkit
 *
 * Application pour créer des objets pour BeHappy
 *
 * Classe principale
**/

#include "3DMTApp.h"
#include "Mainwin.h"
#include "MTPoints.h"
#include "MTVector.h"
#include "MTAddOn.h"
#include "Preferences.h"
#include <FindDirectory.h>
#include <Alert.h>
#include <Path.h>
#include <Directory.h>

Preferences *g_Preferences;

int main()
{
	TDMTApp myApp;
	
	myApp.Run();

	return 0;
}

TDMTApp::TDMTApp()
	: BApplication("application/x-vnd.STertois-3dMorph.Toolkit")
{
}

TDMTApp::~TDMTApp()
{
}

void TDMTApp::ReadyToRun()
{
	Init();
	
	new MainWin(BRect(100,100,300,300));
}

void LaunchWithAddOn(MTAddOnType type, MTObject *object)
{
	TDMTApp::Init();
	
	if ((type & MT_TOOLKIT) != 0)
		(new MainWin(BRect(100,100,300,300)))->InstallObject(1,object);
}

void TDMTApp::Init()
{
	// chargement des préférences
	BPath l_PrefsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY,&l_PrefsPath) == B_OK)
	{
		BDirectory l_SettingsDir(l_PrefsPath.Path());
		if (l_SettingsDir.InitCheck() == B_OK)
		{
			if (!l_SettingsDir.Contains("3DMorph"))
				l_SettingsDir.CreateDirectory("3DMorph",NULL);
			
			l_PrefsPath.Append("3DMorph/ToolKit");
			
			g_Preferences = new Preferences(l_PrefsPath.Path());
			
			if (g_Preferences->InitCheck() != B_OK)
				(new BAlert("3DM Toolkit","Can't open settings file!","Too bad",
					NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			return;
		}
	}
	
	(new BAlert("3DM Toolkit","Can't find the settings directory!",
		"Too bad",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
}

bool TDMTApp::QuitRequested()
{
	g_Preferences->WaitForEnd();
	
	delete g_Preferences;

	return true;
}