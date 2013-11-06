/* écran de veille 3dMorph */

/* commencé le 15 décembre 1998 par Sylvain Tertois */
/* adapté pour BeOs R4.5 le 13 juillet 1999 (Be R4.5 est installé depuis 2h !! ;-) */

/* version 1.00 terminée le 21/09/1999 */
/* version 1.01: 22/10/1999: ajout du cache objets, et introduction d'un double buffering un peu lent */

//#define ALPHA_PPC

#include "list3D.h"
#include "matrix3D.h"
#include "morphing3D.h"
#include "resource3D.h"
#include "morphR45.h"
#include "MorphConfig.h"
#include "MailView.h"
#include "MorphObjects.h"
#include "ScreenConfig.h"
#include "MyScreen.h"

#include <InterfaceKit.h>
#include <stdlib.h>
#include <Beep.h>
#include <MessageRunner.h>
#include <OS.h>

const rgb_color white = { 255,255,255,0 };
const rgb_color black = { 0,  0,  0,  0 };

static float spddefs[PREFSIZE]={ 0.1, 0.5, 0.2,0.5, 0, 0.5, 0.5, 0.5};

extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *msg, image_id image)
{
	return(new MorphSaver(msg,image));
}

//int32 saverVersionID;

MorphSaver::MorphSaver(BMessage *msg, image_id image)
	: BScreenSaver(msg,image)
{
	const float *settings;
	ssize_t settingsSize;
	
// récupération de la config du module
	if ((msg->FindData("speeds",B_FLOAT_TYPE,(const void **)&settings,&settingsSize) == B_OK) &&
			(settingsSize == PREFSIZE*sizeof(float)))
		for (unsigned int i=0; i<PREFSIZE; i++)
			speeds[i]=settings[i];
	else
		for (unsigned int i=0; i<PREFSIZE; i++)
			speeds[i]=spddefs[i];
		
// récupération du fichier resource
	myR3D = new Resource3D(image);
	
	myBitmap = NULL;
	
	mode = true;

// chargement de la config resource
	bool *configResource;
	if ((msg->FindData("active",B_BOOL_TYPE,(const void **)&configResource,&settingsSize) == B_OK) &&
		(settingsSize == (ssize_t)(myR3D->NumRealObjects()*sizeof(bool))))
		myR3D->LoadConfig(configResource);

// récupération du flag "wait for retrace"
	if (msg->FindBool("Wait for Retrace",&waitForRetrace) != B_OK)
		waitForRetrace = true;

// récupération du flag "double buffering"
	if (msg->FindBool("Double Buffering",&draw.doubleBuffering) != B_OK)
		draw.doubleBuffering = true;

// récupération du flag "sofware double buffering"
	if (msg->FindBool("Software Buffering",&draw.softwareDouble) != B_OK)
		draw.softwareDouble = true;

// récupération du type d'écran
	if (msg->FindInt32("Screen space",(int32*)&screenSpace) != B_OK)
		screenSpace = B_16_BIT_640x480;
	
// récupération du type de dessin
	if (msg->FindBool("Nice Draw",&draw.m_NiceDraw) != B_OK)
		draw.m_NiceDraw = true;
	
// récupération de la priorité
//	if (msg->FindInt32("Priority",(int32*)&m_PriorityNum) != B_OK)
//		m_PriorityNum = NORMAL_PRIORITY;
					
	mo = NULL;
	myConfig = NULL;
	sc = NULL;
	screen = NULL;
	morphLooper = NULL;
}

MorphSaver::~MorphSaver()
{
	delete myR3D;
}

status_t MorphSaver::SaveState(BMessage *msg) const
{
	float states[PREFSIZE];
	for (unsigned int i=0; i<PREFSIZE; i++)
		states[i] = speeds[i];
		
	if (speeds[0] == 2.0)
		states[0] = saveSpeed0;
			
	msg->AddData("speeds",B_FLOAT_TYPE,states,PREFSIZE*sizeof(float));

	if (mo != NULL)
		mo->PutConfig();
		
	bool *resourceConfig = new bool[myR3D->NumRealObjects()];
	myR3D->SaveConfig(resourceConfig);
	msg->AddData("active",B_BOOL_TYPE,resourceConfig,myR3D->NumRealObjects()*sizeof(bool));
	delete resourceConfig;
	
	if (sc != NULL)
		sc->SaveState();

	msg->AddBool("Wait for Retrace",waitForRetrace);
	msg->AddBool("Double Buffering",draw.doubleBuffering);
	msg->AddBool("Software Buffering",draw.softwareDouble);
	msg->AddInt32("Screen space",screenSpace);
	msg->AddBool("Nice Draw",draw.m_NiceDraw);
//	msg->AddInt32("Priority",m_PriorityNum);
		
	return B_OK;
}

void MorphSaver::StartConfig(BView *inView)
{
// Construction du tab
	BRect r = inView->Bounds();
	
	BTabView *theTabView = new BTabView(r,NULL);
	theTabView->SetViewColor(216,216,216,0);
	r = theTabView->Bounds();
	r.InsetBy(5,5);
	r.bottom -= theTabView->TabHeight();
	
	// vue info
	BView *infoView = new BView(r,NULL,B_FOLLOW_ALL,B_WILL_DRAW);
	infoView->SetViewColor(216,216,216,0);
	
	BRect frame = infoView->Bounds();
	frame.left += 10.0;
	frame.top += 30;
	frame.bottom = frame.top + 15.0;
#ifndef ALPHA_PPC
	BStringView *view = new BStringView(frame, NULL, "3D Morph 1.10");
#else
	BStringView *view = new BStringView(frame, NULL, "3D Morph 1.10 PPC alpha");
#endif
	infoView->AddChild(view);
	view->SetFont(be_bold_font);
	
	frame.OffsetBy(10,20);
	view = new BStringView(frame,NULL, "... by Sylvain Tertois");
	infoView->AddChild(view);

	frame.OffsetBy(0,20);
	MailView *mail = new MailView(frame,"3dmorph@becoz.org");
	infoView->AddChild(mail);

	frame.OffsetBy(-15,40);
	view = new BStringView(frame,NULL, "3D Morph is mailware!!!");
	infoView->AddChild(view);
	
	frame.OffsetBy(0,20);
	view = new BStringView(frame,NULL, "If you like it please send me an e-mail!");
	infoView->AddChild(view);
	
	frame.OffsetBy(0,20);
	view = new BStringView(frame,NULL, "If you make some nice 3D objects I'm");
	infoView->AddChild(view);

	frame.OffsetBy(0,20);
	view = new BStringView(frame,NULL, " interested too.");
	infoView->AddChild(view);
			
	BTab *tab = new BTab();
	theTabView->AddTab(infoView,tab);
	tab->SetLabel("Info");

	// config
	BView *configView = new BView(r,NULL,B_FOLLOW_ALL,B_WILL_DRAW);
	configView->SetViewColor(216,216,216,0);
	myConfig = new MorphConfig(configView,speeds);
	tab = new BTab();
	theTabView->AddTab(configView,tab);
	tab->SetLabel("Config");
	
	inView->AddChild(theTabView);
	theTabView->SetTabWidth(B_WIDTH_FROM_WIDEST);
	
	// objets 3D
	mo = new MorphObjects(r,myR3D,this);
	mo->SetViewColor(216,216,216,0);
	ObjectsTab *objtab = new ObjectsTab(this,speeds,mo);
	theTabView->AddTab(mo,objtab);
	objtab->SetLabel("Objects");
	
	// config drawing
	sc = new ScreenConfig(r,this);
	tab = new BTab();
	theTabView->AddTab(sc,tab);
	tab->SetLabel("Screen");
}

void MorphSaver::StopConfig()
{
	delete myConfig;
	myConfig = NULL;
	
	sc->SaveState();
	sc = NULL;
	
	mo = NULL;
}

status_t MorphSaver::StartSaver(BView *view, bool prev)
{
	myR3D->LoadCache();
	
	if (sc != NULL)
		sc->SaveState();	
	
	screen = NULL;
	
	srand((long)system_time());
	mView = view;

	if (!view->Window()->Lock())
		return B_ERROR;
				
// Initialisation des coordonnées dans lesquelles on va dessiner
	BRect bounds=view->Bounds();
	draw.width = bounds.IntegerWidth();
	draw.height = bounds.IntegerHeight();
	draw.objectSize = (int)((MIN(draw.width,draw.height)/6)*(0.25+speeds[7]*1.5));
		// speeds[7] est la taille de l'objet
	objectMoveX = draw.width/5;
	objectMoveY = draw.height/5;

// gestion du resource
	status_t file_status;
	
	draw.points3D = obj1 = obj2 = NULL;
	draw.pointsColor = col1 = col2 = NULL;
	obj1Num = NRAND(myR3D->NumObjects());
	obj2Num = NRAND(myR3D->NumObjects());
	file_status = myR3D->GetData(obj2Num,&obj2,&col2);
	ChangeObjects();			// cet appel permet de vraiment choisir deux
								// objets différents et paramètre le type de
								// morphing comme il faut
	if (obj2 == NULL)
		file_status = B_ERROR;
	
	view->Window()->Unlock();
	draw.numOldPoints = draw.numNewPoints = draw.numVoldPoints = 0;
	draw.pNewPoints = draw.pOldPoints = draw.pVoldPoints = NULL;
	draw.points3D = NULL;
	draw.pointsColor = NULL;
	draw.dwDoClip = NULL;
	draw.dwClipList = NULL;
	draw.dwOldDoClip = NULL;

	draw.preview = prev;
	if (draw.preview || (draw.doubleBuffering && draw.softwareDouble))
	{
		myBitmap = new BBitmap(view->Bounds(),myScreen.ColorSpace());
		draw.dwClipList = new clipping_rect;
		draw.dwClipList->left = (int)view->Bounds().left;
		draw.dwClipList->right = (int)view->Bounds().right;
		draw.dwClipList->top = (int)view->Bounds().top;
		draw.dwClipList->bottom = (int)view->Bounds().bottom;
		draw.dwClipNum = 1;
		draw.dwBits = (uint32*)myBitmap->Bits();
		draw.dwRowBytes = myBitmap->BytesPerRow();
		draw.dwFormat = myBitmap->ColorSpace();
		draw.dwtx = draw.width/2;
		draw.dwty = draw.height/2;

		uint32 *erase = draw.dwBits;
		unsigned int n = myBitmap->BitsLength()/4;
		for (unsigned int i=0; i<n; i++)
			*erase++ = 0;
	}
	else
		morphLooper = new MorphLooper(this);

	startTime = system_time();

	// mise à zéro du hook pour le dessin du rectangle
#ifdef _USE_HOOK
	draw.m_DrawRectHook = NULL;
	draw.m_SyncHook = NULL;
#endif
	
	return file_status;
}

void MorphSaver::StopSaver()
{
	if (screen != NULL)
	{
		screen->PostMessage(B_QUIT_REQUESTED);
		screen = NULL;
	}
		
	if (mView->Window()->Lock())
	{
		delete draw.pNewPoints;
		delete draw.pOldPoints;
		delete draw.pVoldPoints;
		delete draw.points3D;
		delete draw.pointsColor;
	
		delete obj1;
		delete obj2;
		delete col1;
		delete col2;
		
		delete draw.dwDoClip;
		delete draw.dwOldDoClip;
	
		mView->Window()->Unlock();
	}
	if (myBitmap != NULL)
	{
		delete myBitmap;
		delete draw.dwClipList;
	}
	if (morphLooper != NULL)
	{
		if (!morphLooper->m_IsStuck)
		{
			morphLooper->PostMessage(B_QUIT_REQUESTED);
			morphLooper = NULL;
		}
		else
		{
			beep();
			kill_team(morphLooper->Team());
		}
	}

	myR3D->ClearCache();
}

void MorphSaver::Draw(BView *theView, int32 frame)
{
	draw.frameNum = frame;

	if (mView->Window()->Lock())
	{
// si c'est la première image, on fait un écran noir...
		if ((frame==0) && !draw.preview)
		{
			mView->SetHighColor(black);
			mView->FillRect(mView->Bounds());

// si on veut du double buffering, il faut ouvrir une BWindowScreen par dessus la fenêtre du screen saver			
			if (draw.doubleBuffering && !draw.softwareDouble)
			{
				// Merci beaucoup à Stephan Assmus (gottgeac@calvados.zrz.TU-Berlin.DE)
				// pour les quelques lignes de code qui suivent...

				// if screen_blanker's BDirectWindow is in full screen mode, we cannot
				// open our BWindowScreen, so set it to window mode
				if (BDirectWindow *win = dynamic_cast<BDirectWindow *>(theView->Window()))
				{
					win->SetFullScreen(false);
				// voilà... c'est ce que j'ai cherché pdt 2 semaines!!!
				
					win->Hide();
				}
				
				// on confie la création d'une BWindowScreen à un autre thread
				// parce que des fois ça foire (par exemple lorsqu'un menu contextuel était actif)
				screen = NULL;
				if (morphLooper != NULL)
				{
/*					BMessageRunner *l_MessageRunner = new BMessageRunner(
						BMessenger(morphLooper),new BMessage('Crea'),10000000,1);
					if (l_MessageRunner->InitCheck() != B_OK)
						beep();
*/					morphLooper->PostMessage('Crea');
				}
				else
					beep();	
			}
		}
		
		if (draw.preview || (draw.doubleBuffering && draw.softwareDouble))
		{
			mView->DrawBitmap(myBitmap);
			DoDraw();
			DoUpdate();
		}

		if (draw.doubleBuffering && !draw.softwareDouble)
		{
			if (screen != NULL)
			{
				acquire_sem(screen->sync);
				screen->videoAccess.Lock();
				DoDraw();
				morphLooper->PostMessage('CALC');
				screen->videoAccess.Unlock();
				BMessage message('DRAW');
				message.AddInt32("image num",frame%2);
				screen->PostMessage(&message);
			}
		}
		
		mView->Window()->Unlock();
	}
}

void MorphSaver::DirectConnected(direct_buffer_info *info)
{
	if (!draw.preview && !draw.doubleBuffering)
	{
		draw.dwBits = (uint32*)(info->bits);
		draw.dwRowBytes = info->bytes_per_row;
		draw.dwFormat = info->pixel_format;
		draw.dwClipNum = info->clip_list_count;
	
		delete draw.dwClipList;
		draw.dwClipList = new clipping_rect[draw.dwClipNum];
		memcpy(draw.dwClipList,info->clip_list,draw.dwClipNum*sizeof(clipping_rect));
	
		draw.width = info->window_bounds.right-info->window_bounds.left;
		draw.height = info->window_bounds.bottom-info->window_bounds.top;
		draw.objectSize = MIN(draw.width,draw.height)/6;
		objectMoveX = draw.width/5;
		objectMoveY = draw.height/5;
	
		draw.dwtx = info->window_bounds.left+draw.width/2;
		draw.dwty = info->window_bounds.top+draw.height/2;
	}
}

void MorphSaver::IsConnected(bool connected, void *buffer, uint32 space)
{
	if (connected)
	{
		switch(ScreenConfig::GetResolution(space))
		{
			case S_640x480:
				draw.width = 640;
				draw.height = 480;
				break;

			case S_800x600:
				draw.width = 800;
				draw.height = 600;
				break;

			case S_1024x768:
				draw.width = 1024;
				draw.height = 768;
				break;

			case S_1152x900:
				draw.width = 1152;
				draw.height = 900;
				break;

			case S_1280x1024:
				draw.width = 1280;
				draw.height = 1024;
				break;

			case S_1600x1200:
				draw.width = 1600;
				draw.height = 1200;
				break;
			
			default:
				draw.width = 640;
				draw.height = 480;
		}
		
		switch(ScreenConfig::GetBitsPPixel(space))
		{
			case 8:
				draw.dwFormat = B_CMAP8;
				draw.dwRowBytes = draw.width;
				break;
			
			case 15:
				draw.dwFormat = B_RGB15;
				draw.dwRowBytes = draw.width*2;
				break;

			case 16:
				draw.dwFormat = B_RGB16;
				draw.dwRowBytes = draw.width*2;
				break;
			
			case 32:
				draw.dwFormat = B_RGB32;
				draw.dwRowBytes = draw.width*4;
				break;
			
			default:
				draw.dwFormat = B_RGB16;
				draw.dwRowBytes = draw.width*2;
		}
			
		draw.dwBits = (uint32*)buffer;
		draw.dwClipNum = 1;
		
		delete draw.dwClipList;
		draw.dwClipList = new clipping_rect;
		draw.dwClipList->top = draw.dwClipList->left = 0;
		draw.dwClipList->right = draw.width;
		draw.dwClipList->bottom = draw.height;
		
		draw.objectSize = (int)((MIN(draw.width,draw.height)/6)*(0.25+speeds[7]*1.5));
			// speeds[7] est la taille de l'objet
		objectMoveX = draw.width/5;
		objectMoveY = draw.height/5;

		draw.dwtx = draw.width/2;
		draw.dwty = draw.height/2;
		
		be_app->HideCursor();
	}
	else
		be_app->ShowCursor();
}
		
void MorphSaver::DirectDraw(int32 frame)
{
	if (!(draw.preview || (draw.doubleBuffering && draw.softwareDouble)))
	{
		if (waitForRetrace)
		{
			do
				myScreen.WaitForRetrace();
			while (morphLocker.CountLocks()!=0);
		}
		else
			morphLocker.Lock();

		if (!draw.doubleBuffering)
		{
			DoDraw();
			morphLooper->PostMessage('CALC');
		}

		if (!waitForRetrace)
			morphLocker.Unlock();
	}	
}

void MorphSaver::DoDraw()
{
	draw.depth = speeds[6];

	if (draw.numOldPoints+draw.numNewPoints == 0)
		return;
	
	// mode 9 pixels
	if (draw.m_NiceDraw)
	{
		switch (draw.dwFormat)
		{
			case B_RGB32:
			case B_RGBA32:
			case B_RGB32_BIG:
			case B_RGBA32_BIG:
				draw.DoDraw3dMorph329();
			break;
			
			case B_RGB16:
			case B_RGB16_BIG:
				draw.DoDraw3dMorph169();
			break;
			
			case B_RGB15:
			case B_RGBA15:
			case B_RGB15_BIG:
			case B_RGBA15_BIG:
				draw.DoDraw3dMorph159();
			break;
				
			case B_CMAP8:
			case B_GRAY8:
				// on récupère la colormap
				draw.map = myScreen.ColorMap()->index_map;

				draw.DoDraw3dMorph089();
			break;
			
			default:
			break;
		}
	}
	else
	// mode 5 pixels
	{
		switch(draw.dwFormat)
		{
			case B_RGB32:
			case B_RGBA32:
			case B_RGB32_BIG:
			case B_RGBA32_BIG:
				draw.DoDraw3dMorph325();
			break;
		
			case B_RGB16:
			case B_RGB16_BIG:
				draw.DoDraw3dMorph165();
			break;
			
			case B_RGB15:
			case B_RGBA15:
			case B_RGB15_BIG:
			case B_RGBA15_BIG:
				draw.DoDraw3dMorph155();
			break;
		
			case B_CMAP8:
			case B_GRAY8:
				// on récupère la colormap
				draw.map = myScreen.ColorMap()->index_map;

				draw.DoDraw3dMorph085();
			break;
		
			default:
			break;
		}
	}
}

BBitmap *MorphSaver::DoBitmap(BRect bounds)
{
	BBitmap *result = new BBitmap(bounds,B_RGB32);
	uint32 *bits = (uint32*)result->Bits();
	{
		uint32 *p = bits;
		for ( int i=0; i<result->BitsLength()/4; i++)
			*p++ = 0;
	}
	
	uint32 rowInts32 = (uint32)bounds.Width()+1;
	
	bool *myClip = new bool[draw.numNewPoints];
	float cx = ((float)bounds.Width())/draw.width, cy = ((float)bounds.Height())/draw.height;
	
	if (draw.numNewPoints != 0)
	{
		for (unsigned int i=0; i<draw.numNewPoints; i++)
		{
			bool out = false;
			int32 x = (int32)(cx*(draw.pNewPoints[i].x+draw.dwtx)), y = (int32)(cy*(draw.pNewPoints[i].y+draw.dwty));
			
			if ((x > 0) &&
				(x < bounds.right) &&
				(y > 0) &&
				(y < bounds.bottom))
				out = true;
			
			if (out)
			{
				uint32 *pt = (uint32*)(bits+y*rowInts32+x);
				uint32 dcol = (draw.pointsColor[i].red<<15 | draw.pointsColor[i].green<<7 | draw.pointsColor[i].blue>>1) & 0x007F7F7F;
				*(pt-1)=dcol;
				*(pt+1)=dcol;
				*(pt-rowInts32)=dcol;
				*(pt+rowInts32)=dcol;
			}
			myClip[i] = out;
		}					
		for (unsigned int i=0; i<draw.numNewPoints; i++)
		{
			if (myClip[i])
			{
				int32 x = (int32)(cx*(draw.pNewPoints[i].x+draw.dwtx)), y = (int32)(cy*(draw.pNewPoints[i].y+draw.dwty));
				*(bits+y*rowInts32+x)= draw.pointsColor[i].red<<16 | draw.pointsColor[i].green<<8 | draw.pointsColor[i].blue;
			}
		}
	}
	
	delete myClip;
	return result;
}

void MorphSaver::DoUpdate()
{
/* échange des 'nouveaux' et 'anciens' points */
	delete draw.pVoldPoints;
	draw.pVoldPoints = draw.pOldPoints;
	draw.pOldPoints = draw.pNewPoints;
	draw.numVoldPoints = draw.numOldPoints;
	draw.numOldPoints = draw.numNewPoints;

/* création des nouveaux points */
	float rmorphTime = (float)(system_time()-startTime)/1000000.0*speeds[0];
	float morphTime = rmorphTime;
	if (morphTime > 3.1416)
		morphTime = 3.1416;
	float rotateTime = (float)system_time()/1000000.0*speeds[1];
	float moveTime = (float)system_time()/1000000.0*speeds[2];
	
	Morphing(morphTyp,(1.0-cos(morphTime))*0.5,obj1,col1,obj2,col2,&draw.points3D,&draw.pointsColor);
	draw.numNewPoints = draw.points3D->NumPoints();
#ifndef ALPHA_PPC
	draw.pNewPoints = new BPoint[draw.numNewPoints];	
#else
	draw.pNewPoints = (BPoint*)(new char[draw.numNewPoints*sizeof(BPoint)]);
#endif
	
/* transformations */
	Matrix3D myMatrix;
	myMatrix.Scale(draw.objectSize*(1+0.2*sin(moveTime*1.15)));
	myMatrix.RotateY(1.2*rotateTime);
	myMatrix.RotateX(1.0*rotateTime);
	myMatrix.RotateZ(0.8*rotateTime);
	myMatrix.Translate(objectMoveX*sin(moveTime*0.85),objectMoveY*cos(moveTime*1.0),0);
	draw.points3D->Transform(myMatrix);

/* projection */	
	draw.points3D->Project(-draw.objectSize*6*(2.5-2*speeds[5]),draw.pNewPoints);

/* si le morphing actuel est fini, il faut en faire un autre*/
	if (((rmorphTime - speeds[4]*60*speeds[0])>=3.1415) && mode)	// speeds[4] c'est le temps de pause
		ChangeObjects();	// sauf si on est en mode bloqué
}

void MorphSaver::ChangeObjects()
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

void MorphSaver::BlockOnObject(unsigned int n)
{
	BString dummy;
	
// met l'état actuel du morphing comme objet 1
	float morphTime = (float)(system_time()-startTime)/1000000.0*speeds[0];
	if (morphTime > 3.1416)
		morphTime = 3.1416;
	List3D *p=NULL;
	rgb_color *c=NULL;

	Morphing(morphTyp,(1.0-cos(morphTime))*0.5,obj1,col1,obj2,col2,&p,&c);
	draw.numNewPoints = p->NumPoints();
	delete obj1;
	obj1 = p;
	delete col1;
	col1 = c;
	
// prend le nouvel objet comme objet2
	myR3D->GetData(n,&obj2,&col2,false);

// détermination du type de morphing
	if (((float)NRAND(100))/100>speeds[3])
		morphTyp = M3D_UNIFORM;
	else
		morphTyp = M3D_PROGRESSIVE;

	startTime = system_time();
	mode = false;
}

void MorphSaver::Unblock()
{
	BString dummy;
	
// met l'état actuel du morphing comme objet 1
	float morphTime = (float)(system_time()-startTime)/1000000.0*speeds[0];
	if (morphTime > 3.1416)
		morphTime = 3.1416;
	List3D *p=NULL;
	rgb_color *c=NULL;

	Morphing(morphTyp,(1.0-cos(morphTime))*0.5,obj1,col1,obj2,col2,&p,&c);
	draw.numNewPoints = p->NumPoints();
	delete obj2;		// en fait c'est stoqué provisoirement dans l'objet2
	obj2 = p;			// parce que ChangeObjects() ci-dessous
	delete col2;		// va faire l'échange
	col2 = c;			//
	
// prend un nouvel objet comme objet2
	ChangeObjects();

	mode = true;
}

void MorphSaver::ToggleSpeed0(bool what)
{
	if (what)
	{
		saveSpeed0 = speeds[0];
		speeds[0] = 2.0;
	}
	else
	{
		speeds[0] = saveSpeed0;
	}
}

MorphLooper::MorphLooper(MorphSaver *ms)
{
	saver = ms;
	Run();
	m_IsStuck = false;
}

void MorphLooper::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'CALC':
			saver->morphLocker.Lock();
			saver->DoUpdate();
			saver->morphLocker.Unlock();
			break;
		
		case 'Crea':
		{
			status_t error;
			m_IsStuck = true;
			MyScreen *l_Screen = new MyScreen(saver,&error,saver->screenSpace);
			m_IsStuck = false;
			if (error != B_OK)
				saver->draw.doubleBuffering = false;
			else
				saver->screen = l_Screen;
		}
			break;

		default:
			BLooper::MessageReceived(message);
	}
}
