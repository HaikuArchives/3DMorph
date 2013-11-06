#ifndef _MORPH_H
#define _MORPH_H

#include <InterfaceKit.h>

extern "C" {

extern void	module_initialize(void *inSettings, long inSettingsSize);
extern void	module_cleanup(void **outSettings, long *outSettingsSize);
extern void	module_start_saving(BView *inView);
extern void	module_stop_saving();
extern void module_start_config(BView *inView);
extern void module_stop_config();

}

#include "CSaveThread.h"
#include "list3D.h"
#include "resource3D.h"
#include "morphing3D.h"


#define NRAND(X) ((int)(rand()%(X)))

class CMorphThread : CSaveThread {
public:
					CMorphThread();
					
	virtual void	StartSaving(BView *view);
	virtual void	StopSaving();

protected:
	virtual double	Save();
	void	Draw();
	void	ChangeObjects();

protected:
	double	mSleep;
	long	batchcount;
// données sur la taille de l'écran
	int		width,height,objectSize,objectMoveX,objectMoveY;
	BPoint	middle;
// heure du début du morphing actuel
	bigtime_t startTime;
// points 2D affichés
	BPoint	*pOldPoints,*pNewPoints;
// nombre de points 2D dans les tableaux pOldPoints et pNewPoints
	unsigned int numOldPoints,numNewPoints;
// resource contenant tous les objets 3D
	Resource3D	*myR3D;
// numéros des objets dont on fait le morphing
	unsigned int obj1Num,obj2Num;
// type de morphing en cours
	MorphingType morphTyp;
// couleurs des points 3D affichés en ce moment, et celles des 2 objets dont on fait le morphing
	rgb_color *pointsColor,*col1,*col2;
// coords des points 3D affichés en ce moment, et celles des 2 objets dont on fait la morphing
	List3D	*points3D,*obj1,*obj2;
};

#endif