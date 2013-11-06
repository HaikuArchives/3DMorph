/*
 * 3D Morph Toolkit
 *
 * fenêtre principale
 *
**/

#ifndef MAINWIN_H
#define MAINWIN_H

#include <Window.h>
#include "MTObject.h"
#include "image.h"	// pour image_id

class ViewWindow;
class BMenuItem;
class BStringView;
class BSlider;
class BButton;
class MTObject;
class HappyCommander;

class MainWin : public BWindow
{
public:
	MainWin(BRect frame);
	~MainWin();
	
	virtual bool QuitRequested();
	void RequestUpdate();
	
	virtual void MessageReceived(BMessage *message);
	
	// chergement de nouveaux objets
	void LoadFile(int16 num, entry_ref &ref);
	void InstallObject(int16 num, MTObject *newObject);

	void UpdateViewWindow();	// calcule le morphing s'il le faut, et envoie
								// ce qu'il faut afficher à la fenêtre de vue 3D
	void CalcObjects();			// fait une mise à jour en demandant les nouveaux
								// points aux objets
	
	void SaveFile(entry_ref &directory, const char *name);
		
private:
	ViewWindow *m_Viewer;
	BView *m_MainView;
	
	// items de menu qu'il faut garder
	BMenuItem *m_MILoad2;
	BMenuItem *m_MISave;
	BMenuItem *m_MIUpdate;
	BMenuItem *m_MIBuffering;
	
	// autres éléments de l'interface
	BButton *m_Button;
	BStringView *m_Label;
	BSlider *m_Morphing;
	
	// taille de la partie haute de la fenêtre (la 'toolbar')
	float m_ToolbarHeight;
	
	// les deux objets de la fenêtre
	MTObject *m_Object1, *m_Object2;
	BView *m_Object1View, *m_Object2View;
	MTPoints *m_Points1, *m_Points2;
	image_id m_AddOnID1, m_AddOnID2;
	
	// le panel utilisé pour sauver
	BFilePanel *m_SavePanel;
	
	// commande de BeHappy
	HappyCommander *m_BeHappy;
};

// variable globale qui pointe vers la fenêtre main
extern MainWin *g_MainWin;

// panel utilisé pour charger les objets
extern BFilePanel *g_FilePanel;

#endif MAINWIN_H