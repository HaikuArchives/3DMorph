#ifndef _MORPH_H
#define _MORPH_H

#include "./ScreenSaver.h"
#include "resource3D.h"
#include "morphing3D.h"
#include "Draw3D.h"

#include <Screen.h>

class MorphConfig;
class MorphObjects;
class ScreenConfig;
class MyScreen;

#define NRAND(X) ((int)(rand()%(X)))
extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *msg, image_id image);

class MorphLooper;

#define PREFSIZE 8

class MorphSaver : public BScreenSaver
{
public:
	MorphSaver(BMessage *msg, image_id image);
	~MorphSaver();
	
	status_t InitCheck() { return myR3D->InitCheck(); }				
	status_t SaveState(BMessage*) const;
	
	void StartConfig(BView*);
	void StopConfig();
	
	status_t StartSaver(BView*,bool);
	void StopSaver();
	
	void Draw(BView*,int32);
	void DirectConnected(direct_buffer_info*);
	void IsConnected(bool connected, void *buffer, uint32 space);
	void DirectDraw(int32);

	void BlockOnObject(unsigned int);	// bloque sur un objet
	void Unblock();	// remet en mode normal
	void ToggleSpeed0(bool);	// true: met une vitesse morphing de 2.0
								// false: remet l'ancienne valeur

	BBitmap *DoBitmap(BRect);
		
protected:
		// dessin
	void DoDraw();
		// ... avec les différents plans de bits
/*	void DoDraw32();
	void DoDraw16();
	void DoDraw15();
	void DoDraw8();
*/	
		// calculs
	void DoUpdate();
	void ChangeObjects();	// appelé à la fin d'un morphing, pour prendre un nouvel objet

protected:
// configuration
	MorphConfig *myConfig;
	float speeds[PREFSIZE];
	float saveSpeed0;
	MorphObjects *mo;
	bool waitForRetrace;
// mode normal (true) ou à objet constant (false)
	bool mode;
// vue utilisée pour l'écran de veille
	BView *mView;
	BBitmap *myBitmap;
	BScreen myScreen;
// données sur la taille de l'écran
	int		width,objectMoveX,objectMoveY;
// heure du début du morphing actuel
	bigtime_t startTime;
// resource contenant tous les objets 3D
	Resource3D	*myR3D;
// numéros des objets dont on fait le morphing
	unsigned int obj1Num,obj2Num;
// type de morphing en cours
	MorphingType morphTyp;
// couleurs des points 3D affichés en ce moment, et celles des 2 objets dont on fait le morphing
	rgb_color *col1,*col2;
// coordonnées des points 3D ades 2 objets dont on fait le morphing
	List3D	*obj1,*obj2;
// paramètres pour le thread de calcul
	MorphLooper *morphLooper;
	BLocker morphLocker;
// paramètres d'affichage
	ScreenConfig *sc;
	uint32 screenSpace;
	MyScreen *screen;
// classe qui fait le dessin
	Draw3D draw;
	
	friend class MorphLooper;
	friend class ScreenConfig;
};

class MorphLooper : public BLooper
{
public:
	MorphLooper(MorphSaver*);
	
	void MessageReceived(BMessage *message);

	bool m_IsStuck;		// true si le looper est coincé dans la création de la BWindowScreen
	
private:
	MorphSaver *saver;
};


#endif