/*
 * View Window
 *
 * Fenêtre qui affiche un objet 3D
 *
**/
 
#ifndef VIEWWIN_H
#define VIEWWIN_H

#include <DirectWindow.h>
#include <Locker.h>
#include <View.h>
#include <Screen.h>
#include "Draw3D.h"
#include <kernel/OS.h>	// pour le sémaphore
#include "MTMatrix.h"

class MTPoints;
class TDMView;
class BBitmap;
class BStringView;

class ViewWindow : public BDirectWindow
{
public:
	ViewWindow(BRect frame);
	~ViewWindow();
	
	virtual bool QuitRequested();
	virtual void DirectConnected(direct_buffer_info *info);
	
	void NewPoints(MTPoints*);
		// change les points à afficher
	
	// synchro avec le thread de dessin
	bool m_ThreadEnd, m_Connected;
	thread_id m_ThreadID;
	BLocker m_Locker;					// verrou pour les données vidéo
	static int32 DrawingThread(void*);
	
	// classe de dessin
	Draw3D m_Draw3D;
	
	// objet à dessiner
	MTPoints *m_Points;

	// verrou supplémentaire pour l'accès aux données 3D (m_Points ci-dessus)
	BLocker m_3DDataLock;
	
	// flag mis à true quand la fenêtre doit être entièrement redessinée
	bool m_Clean;
	
	// paramètres qui contrôle les rotations de l'objet
	float m_XAngle, m_XSpeed;
	float m_YAngle, m_YSpeed;
	bigtime_t m_LastTime;	// heure au dernier calcul (sert à déterminer
							// le temps qui s'est écoulé depuis la dernière
							// image)

	// objet BScreen pour la synchro
	BScreen m_Screen;
	
	// objet bitmap pour le double buffering en soft
	BBitmap *m_Bitmap;
	
	// matrice de base pour la transformation de l'objet affiché
	MTMatrix m_BaseMatrix;
		
private:
	TDMView *m_View;
	BStringView *m_StringView;
	
	// sémaphore utilisé pour éviter qu'on change plusieurs fois l'objet à
	// afficher entre deux dessins
	sem_id m_ChangeSema;
};

class TDMView : public BView
{
public:
	TDMView(BRect frame, ViewWindow *window);
	~TDMView();
	
	virtual void MouseDown(BPoint);
	virtual void MouseMoved(BPoint,uint32,const BMessage*);
	virtual void MouseUp(BPoint);
	
private:
	ViewWindow *m_Window;
	float m_OldX, m_OldY;	// sauvegarde des coordonnées de la souris quand
							// on clique dans la vue
	bigtime_t m_OldTime;	// heure au moment où on a pris ces coordonnées
	float m_VOldX, m_VOldY;	// pareil en plus vieux
	bigtime_t m_VOldTime;
	bool m_Clic;			// true si l'utilisateur a appuyé sur le bouton
};

#endif //VIEWWIN_H