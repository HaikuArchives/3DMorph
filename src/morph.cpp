/* écran de veille 3dMorph */

/* commencé le 15 décembre 1998 par Sylvain Tertois */

#include "list3D.h"
#include "matrix3D.h"
#include "morphing3D.h"
#include "resource3D.h"
#include "morph.h"

#include <stdlib.h>

CMorphThread	*gMorph = NULL;

const rgb_color white = { 255,255,255,0 };
const rgb_color black = { 0,  0,  0,  0 };


//static float speeds[4]={ 0.1, 0.5, 0.2,0.5 };
static float speeds[4]={1.0,0.5,0.2,0.3};

BSlider *config1,*config2,*config3,*config4;

void
module_initialize(
	void	*inSettings,
	long 	inSettingsSize)
{
	if (inSettingsSize == 4*sizeof(float))
	{
		speeds[0]=((float*)inSettings)[0];
		speeds[1]=((float*)inSettings)[1];
		speeds[2]=((float*)inSettings)[2];
		speeds[3]=((float*)inSettings)[3];
	}
}


void
module_cleanup(
	void	**outSettings,
	long	*outSettingsSize)
{
	*outSettings = new float[4];
	((float*)*outSettings)[0]=speeds[0];
	((float*)*outSettings)[1]=speeds[1];
	((float*)*outSettings)[2]=speeds[2];
	((float*)*outSettings)[3]=speeds[3];
	*outSettingsSize = (long)(4*sizeof(float));
}


void
module_start_saving(
	BView	*inView)
{	
	srand((long)system_time());

	inView->Window()->Show();

	gMorph = new CMorphThread();
	gMorph->StartSaving(inView);	
}


void
module_stop_saving()
{
	gMorph->StopSaving();

	delete (gMorph);
	gMorph = NULL;
}


void
module_start_config(
	BView *inView)
{
	if (!inView->Window()->Lock())
		return;
		
	BRect frame;
	
	frame = inView->Bounds();
	frame.left += 10.0;
	frame.bottom = frame.top + 15.0;
	BStringView *view = new BStringView(frame, B_EMPTY_STRING, "3D Morph");
	inView->AddChild(view);
	view->SetViewColor(inView->ViewColor());
	view->SetFont(be_bold_font);
	
	frame = inView->Bounds();
	frame.top += 15.0;
	frame.bottom = frame.top + 50.0;
	frame.left += 5.0;
	frame.right -= 5.0;
	BRect textArea = frame;
	textArea.OffsetTo(B_ORIGIN);
	BTextView *caption = new BTextView(frame, B_EMPTY_STRING, textArea,
						   			   B_FOLLOW_ALL, B_WILL_DRAW);
	caption->SetText("by Sylvain Tertois <sylvain.tertois@supelec.fr>");
	inView->AddChild(caption);
	caption->SetViewColor(inView->ViewColor());
	caption->SetDrawingMode(B_OP_OVER);
	caption->SetFont(be_plain_font);
	caption->MakeEditable(FALSE);
	caption->MakeSelectable(FALSE);
	caption->SetWordWrap(TRUE);	
	
	frame.top += 30.0;
	frame.bottom = frame.top + 50.0;
	config1 = new BSlider(frame,B_EMPTY_STRING,"Morphing speed",NULL,0,100);
	inView->AddChild(config1);
	config1->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config1->SetHashMarkCount(11);
	config1->SetLimitLabels("Slow", "Fast");
	config1->SetPosition(speeds[0]);
	config1->SetKeyIncrementValue(10);
	
	frame.top += 50.0;
	frame.bottom = frame.top + 50.0;
	config2 = new BSlider(frame,B_EMPTY_STRING,"Mouvement speed",NULL,0,100);
	inView->AddChild(config2);
	config2->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config2->SetHashMarkCount(11);
	config2->SetLimitLabels("Slow", "Fast");
	config2->SetPosition(speeds[1]);
	config2->SetKeyIncrementValue(10);
	
	frame.top += 50.0;
	frame.bottom = frame.top + 50.0;
	config3 = new BSlider(frame,B_EMPTY_STRING,"Rotation speed",NULL,0,100);
	inView->AddChild(config3);
	config3->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config3->SetHashMarkCount(11);
	config3->SetLimitLabels("Slow", "Fast");
	config3->SetPosition(speeds[2]);
	config3->SetKeyIncrementValue(10);
	
	frame.top += 50.0;
	frame.bottom = frame.top + 50.0;
	config4 = new BSlider(frame,B_EMPTY_STRING,"Morphing Type",NULL,0,100);
	inView->AddChild(config4);
	config4->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config4->SetHashMarkCount(11);
	config4->SetLimitLabels("Uniform", "Progressive");
	config4->SetPosition(speeds[3]);
	config4->SetKeyIncrementValue(10);
	
	inView->Window()->UpdateIfNeeded();
	
	inView->Window()->Unlock();
	
}


void
module_stop_config()
{
	speeds[0] = config1->Position()+0.01;
	speeds[1] = config2->Position()+0.01;
	speeds[2] = config3->Position()+0.01;
	speeds[3] = config4->Position();
}




CMorphThread::CMorphThread()
	: CSaveThread()
{
	mSleep = 25000.0;
	batchcount=-7;
}


void CMorphThread::StartSaving(BView	*view)
{
	if (!view->Window()->Lock())
		return;

// Initialisation des coordonnées dans lesquelles on va dessiner
	BRect bounds=view->Bounds();
	width = bounds.IntegerWidth();
	height = bounds.IntegerHeight();
	middle.Set(width/2,height/2);
	objectSize = MIN(width,height)/6;
	objectMoveX = width/4;
	objectMoveY = height/4;

// gestion du resource
	myR3D = new Resource3D;
	status_t file_status;
	
// si le fichier à l'air ok, on le charge
	if ( (file_status=myR3D->InitCheck()) == B_OK )
	{
		points3D = obj1 = obj2 = NULL;
		pointsColor = col1 = col2 = NULL;
		obj1Num = 1;
		obj2Num = 2;
		morphTyp = M3D_UNIFORM;
		file_status=myR3D->GetData(obj1Num,&obj1,&col1);
		if (file_status == B_OK)
			file_status = myR3D->GetData(obj2Num,&obj2,&col2);
	}

	view->Window()->Unlock();
	numOldPoints = numNewPoints = 0;
	pNewPoints = pOldPoints = NULL;
	points3D = NULL;
	pointsColor = NULL;
	startTime = system_time();
	
// si le fichier n'est pas bon, on ne fait rien
	if (file_status==B_OK)
		CSaveThread::StartSaving(view);	
}

	
void
CMorphThread::StopSaving()
{
	CSaveThread::StopSaving();

	if (mView->Window()->Lock())
	{
		if (pNewPoints != NULL)
			delete pNewPoints;
		if (pOldPoints != NULL)
			delete pOldPoints;
		if (points3D != NULL)
			delete points3D;
		if (pointsColor != NULL)
			delete pointsColor;
	
		delete obj1;
		delete obj2;
		delete col1;
		delete col2;
	
		delete myR3D;

		mView->Window()->Unlock();
	}
}

	
double CMorphThread::Save()
{
	if (mView->Window()->Lock())
	{
		Draw();
		
/* échange des 'nouveaux' et 'anciens' points */
		if (pOldPoints != NULL)
			delete pOldPoints;
		pOldPoints = pNewPoints;
		numOldPoints = numNewPoints;

/* création des nouveaux points */
		float morphTime = (float)(system_time()-startTime)/1000000.0*speeds[0];
		float rotateTime = (float)system_time()/1000000.0*speeds[1];
		float moveTime = (float)system_time()/1000000.0*speeds[2];
	
		Morphing(morphTyp,(1.0-cos(morphTime))*0.5,obj1,col1,obj2,col2,&points3D,&pointsColor);
		numNewPoints = points3D->NumPoints();
		pNewPoints = new BPoint[numNewPoints];	
	
/* transformations */
		Matrix3D myMatrix;
		myMatrix.Scale(objectSize);
		myMatrix.RotateY(1.2*rotateTime);
		myMatrix.RotateX(1.0*rotateTime);
		myMatrix.RotateZ(0.8*rotateTime);
		myMatrix.Translate(objectMoveX*sin(moveTime*1.15),objectMoveY*cos(moveTime*1.0),objectMoveY*sin(moveTime*0.85));
		points3D->Transform(myMatrix);

/* projection */	
		points3D->Project(10000,pNewPoints);

/* si le morphing actuel est fini, il faut en faire un autre*/
		if (morphTime>=3.1415)
			ChangeObjects();
		mView->Window()->Unlock();
	}
	return(mSleep);
}

void CMorphThread::Draw()
{
	BScreen theScreen(B_MAIN_SCREEN_ID);
	
	if (numOldPoints+numNewPoints == 0)
		return;

	BPoint point;
	mView->BeginLineArray(numOldPoints+numNewPoints+10);

	theScreen.WaitForRetrace();
		
	if (numOldPoints != 0)
		for (unsigned int i=0; i<numOldPoints; i++)
		{
			point = pOldPoints[i];
			point += middle;
			mView->AddLine(point,point,black);
		}
	if (numNewPoints != 0)
		for (unsigned int i=0; i<numNewPoints; i++)
		{
			point = pNewPoints[i];
			point += middle;
			mView->AddLine(point,point,pointsColor[i]);
		}
	mView->EndLineArray();
	mView->Sync();
}

void CMorphThread::ChangeObjects()
{
	unsigned int newObjNum=obj1Num;

// cherche un nouvel objet
	unsigned int n = myR3D->NumObjects();
		
	while((newObjNum==obj1Num) || (newObjNum==obj2Num))
		newObjNum=NRAND(n);

// le prend dans le resource
	List3D *newObj=NULL;
	rgb_color *newCol=NULL;
	
	if ( myR3D->GetData(newObjNum,&newObj,&newCol) == B_OK )
	{
		delete obj1;
		delete col1;
	}
	else
	{
// il y a eu une erreur lors de l'extraction de l'objet. On garde les mêmes
		newObj = obj1;
		newCol = col1;
		newObjNum = obj1Num;
	}
	obj1 = obj2;
	col1 = col2;
	obj1Num = obj2Num;
	
	obj2 = newObj;
	col2 = newCol;
	obj2Num = newObjNum;

// détermination du type de morphing
	if (((float)NRAND(100))/100>speeds[3])
		morphTyp = M3D_UNIFORM;
	else
		morphTyp = M3D_PROGRESSIVE;
		
	startTime = system_time();
}
	
