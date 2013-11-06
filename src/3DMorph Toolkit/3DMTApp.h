/*
 * 3D Morph Toolkit
 *
 * Application pour créer des objets pour 3DMorph
 *
 * Classe principale
**/

#ifndef TDMTAPP
#define TDMTAPP

#include <Application.h>
#include "MTAddOn.h"

class MTObject;

class TDMTApp : public BApplication
{
public:
	TDMTApp();
	~TDMTApp();
	
	virtual void ReadyToRun();
	virtual bool QuitRequested();
	
	static void Init();
};

// fonction externe pour lancer l'application avec un add-on
extern "C" __declspec(dllexport) void LaunchWithAddOn(MTAddOnType,MTObject*);

// préférences
class Preferences;
extern Preferences *g_Preferences;

#endif //TDMTAPP
