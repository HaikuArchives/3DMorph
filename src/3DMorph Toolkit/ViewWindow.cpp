/*
 * View Window
 *
 * Fenêtre qui affiche un objet 3D
 *
**/
 
#include "ViewWindow.h"
#include <View.h>
#include "MTPoints.h"
#include <Application.h>
#include <string.h>		// pour memset
#include <Bitmap.h>
#include <StringView.h>
#include <String.h>

//#include <stdio.h>

#include "Preferences.h"
#include "3DMTApp.h"

ViewWindow::ViewWindow(BRect frame)
	: BDirectWindow(frame, "3D View", B_DOCUMENT_WINDOW,
		B_ASYNCHRONOUS_CONTROLS|B_WILL_ACCEPT_FIRST_CLICK|B_NOT_CLOSABLE)
{
	g_Preferences->SizeWindow("3D View",this);
	
	m_ThreadEnd = false;
	
	// création d'une vue transparente, pour éviter que l'app_server s'amuse
	// avec le contenu de la fenêtre
	BRect l_Rect(Bounds());
	l_Rect.bottom -= 14;
	m_View = new TDMView(l_Rect,this);
	AddChild(m_View);
	
	// création d'une vue qui affichera des informations
	l_Rect.top = l_Rect.bottom+1;
	l_Rect.bottom = Bounds().bottom;
	m_StringView = new BStringView(l_Rect,NULL,"",
		B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM);
	AddChild(m_StringView);	
	m_StringView->SetViewColor(ui_color(B_MENU_BACKGROUND_COLOR));

	// paramètres par défaut pour le dessin 3D
	m_Draw3D.height = 100;
	m_Draw3D.objectSize = 100;
	m_Draw3D.pVoldPoints = NULL;
	m_Draw3D.pOldPoints = NULL;	
	m_Draw3D.pNewPoints = NULL;	
	m_Draw3D.pointsColor = NULL;	
	m_Draw3D.numVoldPoints = 0;	
	m_Draw3D.numOldPoints = 0;
	m_Draw3D.numNewPoints = 0;	
	m_Draw3D.dwClipList = NULL;	
	m_Draw3D.dwClipNum = 0;	
	m_Draw3D.dwDoClip = NULL;	
	m_Draw3D.dwOldDoClip = 0;	
	m_Draw3D.doubleBuffering = false;	
	m_Draw3D.softwareDouble = true;	
	m_Draw3D.frameNum = 0;	
	m_Draw3D.points3D = NULL;	
	m_Draw3D.preview = false;	
	m_Draw3D.map = NULL;	
	m_Points = NULL;
	m_Clean = false;
	m_XAngle = 0;
	m_XSpeed = 1.0;
	m_YAngle = 0;
	m_YSpeed = 1.2;
	m_LastTime = system_time();
	m_Bitmap = NULL;
	
	// création du sémaphore de dessin
	m_ChangeSema = create_sem(1,"Change Semaphore");
	
	// lancement du thread qui va faire le dessin
	m_ThreadEnd = false;
	m_Connected = false;
	m_ThreadID = spawn_thread(DrawingThread,"drawing thread",B_NORMAL_PRIORITY,
		(void*)this);
	resume_thread(m_ThreadID);
	
	// ouverture de la fenêtre
	Show();	
}

ViewWindow::~ViewWindow()
{
	g_Preferences->CloseWindow(this);
	
	// on prévient le thread qu'on va quitter
	m_ThreadEnd = true;
	
	// on cache la fenêtre, ça va fermer la connection
	Hide();
	Sync();
	int32 result;
	wait_for_thread(m_ThreadID, &result);
	
	// destruction de quelques trucs
	delete m_Draw3D.pVoldPoints;
	delete m_Draw3D.pOldPoints;
	delete m_Draw3D.pNewPoints;
	delete m_Draw3D.pointsColor;
	delete m_Draw3D.dwClipList;
	delete m_Draw3D.dwDoClip;
	delete m_Draw3D.dwOldDoClip;
	delete m_Draw3D.points3D;
	
	delete m_Bitmap;
	
	delete_sem(m_ChangeSema);
}
	
bool ViewWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void ViewWindow::DirectConnected(direct_buffer_info *info)
{
	// on ne fait rien si on est en train de quitter
	if (!m_Connected && m_ThreadEnd)
		return;

	// sinon on commene par bloquer le thread de dessin
	m_Locker.Lock();
	
	// extraction des informations
	switch(info->buffer_state & B_DIRECT_MODE_MASK)
	{
		case B_DIRECT_START:
			m_Connected = true;
		// l'absence de break est volontaire
		
		case B_DIRECT_MODIFY:
			// copie des paramètres
			m_Draw3D.dwFormat = info->pixel_format;
				
			// là ça dépend de si on utilise un bitmap ou non
			if (!m_Draw3D.softwareDouble)
			{
				m_Draw3D.dwBits = (uint32*)info->bits;
				m_Draw3D.dwRowBytes = info->bytes_per_row;
				delete[] m_Draw3D.dwClipList;
				m_Draw3D.dwClipList = new clipping_rect[info->clip_list_count];
				m_Draw3D.dwClipNum = info->clip_list_count;
				memcpy(m_Draw3D.dwClipList,info->clip_list,
					info->clip_list_count*sizeof(clipping_rect));
			}
			
			// si on a le double buffering, la copie des paramètres se fera dans
			// le thread de dessin (parce qu'on a besoin de locker la fenêtre
			// et que ce n'est pas possible ici)
			
			// on prévient le thread de dessin qu'il y a du ménage à faire
			m_Clean = true;
			break;
		
		case B_DIRECT_STOP:
			m_Connected =false;
			break;
	} 

	// c'est bon
	m_Locker.Unlock();
}

int32 ViewWindow::DrawingThread(void *me)
{
	ViewWindow *l_me = (ViewWindow*)me;
	bool bloque;	// true si le locker est encore locké

	while(!l_me->m_ThreadEnd)
	{
		// synchro
		if (!l_me->m_Draw3D.softwareDouble)
			l_me->m_Screen.WaitForRetrace();
		
		// on va commencer à foutre le bordel... on bloque tout
		l_me->m_Locker.Lock();
		bloque = true;

		// si on doit nettoyer l'écran on va le faire ici
		if (l_me->m_Clean)
		{
			// si un bitmap était alloué, on peut l'effacer
			delete l_me->m_Bitmap;
			l_me->m_Bitmap = NULL;
			
			// on doit débloquer le locker qui agit sur les données,
			// mais par contre bloquer celui de la fenêtre pour pouvoir
			// dessiner dedans
			l_me->m_Locker.Unlock();
			l_me->Lock();
			
			if (!l_me->m_Draw3D.softwareDouble)
			{
				l_me->m_View->FillRect(l_me->Bounds());
				l_me->Sync();
			} // si on est en double buffering, le noir sera fait en mettant
			  // tout le bitmap à 0 (voir ci-dessous)

			// et on profite du bloquage de la fenêtre pour récupérer ses
			// coordonnées			
			BRect frame = l_me->m_View->Frame();
			
			// la mise à jour des paramètres dépend un peu de l'utilisation du
			// double buffering
			if (!l_me->m_Draw3D.softwareDouble)
			{
				l_me->m_Draw3D.dwtx = (int32)(frame.left+frame.right)/2;
				l_me->m_Draw3D.dwty = (int32)(frame.top+frame.bottom)/2;
			}
			else
			{
				// allocation d'un nouveau bitmap
				l_me->m_Bitmap = new BBitmap(frame,l_me->m_Draw3D.dwFormat);

				// mise à jour des paramètres de dessin
				l_me->m_Draw3D.dwBits = (uint32*)l_me->m_Bitmap->Bits();
				l_me->m_Draw3D.dwRowBytes = l_me->m_Bitmap->BytesPerRow();
				l_me->m_Draw3D.dwtx = (int32)(frame.Width()/2);
				l_me->m_Draw3D.dwty = (int32)(frame.Height()/2);
				l_me->m_Draw3D.dwClipNum = 1;
				delete l_me->m_Draw3D.dwClipList;
				l_me->m_Draw3D.dwClipList = new clipping_rect;
				l_me->m_Draw3D.dwClipList->left = 0;
				l_me->m_Draw3D.dwClipList->top = 0;
				l_me->m_Draw3D.dwClipList->right = (int32)frame.Width()-1;
				l_me->m_Draw3D.dwClipList->bottom = (int32)frame.Height()-1;
				
				// on met tout à zéro
				memset(l_me->m_Bitmap->Bits(),0,l_me->m_Bitmap->BitsLength());
			}
			
			float size = frame.Width();
			if (frame.Height() < size)
				size = frame.Height();
			size *= 0.45;
			l_me->m_Draw3D.objectSize = (int)size;
			l_me->m_Draw3D.height = (int)frame.Height();

			// on débloque la fenêtre, puis on rebloque sur les données
			l_me->Unlock();
			l_me->m_Locker.Lock();
			
			// la fonction qui dessine ci-dessous n'a pas besoin d'effacer
			// les ancients points, puisqu'on vient de dessiner un gros
			// rectangle noir... donc on efface toutes les données relatives
			// aux anciens points
			
			delete l_me->m_Draw3D.pVoldPoints;
			delete l_me->m_Draw3D.pOldPoints;
			l_me->m_Draw3D.pVoldPoints = NULL;
			l_me->m_Draw3D.pOldPoints = NULL;
			
			l_me->m_Draw3D.numVoldPoints = 0;
			l_me->m_Draw3D.numOldPoints = 0;

			l_me->m_Clean = false;
		}
		
		
		if (l_me->m_Connected)
		{
			// on va dessiner
			if (l_me->m_Draw3D.numOldPoints+l_me->m_Draw3D.numNewPoints != 0)
			{
				l_me->m_3DDataLock.Lock();
				
				l_me->m_Draw3D.depth = 0.75;	

				switch (l_me->m_Draw3D.dwFormat)
				{
					case B_RGB32:
					case B_RGBA32:
						l_me->m_Draw3D.DoDraw3dMorph329();
						break;
			
					case B_RGB16:
						l_me->m_Draw3D.DoDraw3dMorph169();
						break;
				
					case B_RGB15:
					case B_RGBA15:
						l_me->m_Draw3D.DoDraw3dMorph159();
						break;
				
					case B_CMAP8:
					case B_GRAY8:
						{
							// on récupère la colormap
							l_me->m_Draw3D.map = l_me->m_Screen.ColorMap()->index_map;

							l_me->m_Draw3D.DoDraw3dMorph089();
						}
						break;
			
					default:
					break;
				}
				
				l_me->m_3DDataLock.Unlock();
			}
			
			// on peut débloquer
			if (bloque)
			{
				l_me->m_Locker.Unlock();
				bloque = false;
			}
			
			// si on est en double buffering, c'est le moment d'afficher
			if (l_me->m_Draw3D.softwareDouble)
			{
				l_me->Lock();
				l_me->m_View->DrawBitmap(l_me->m_Bitmap);
				l_me->Unlock();
			}
		
			// mise à jour des points
			if (l_me->m_Points != NULL)
			{
				/// verouillage de l'accès aux données
				l_me->m_3DDataLock.Lock();
				
				/// on commence par échanger les 'nouveaux' et les anciens
				delete l_me->m_Draw3D.pVoldPoints;
				l_me->m_Draw3D.pVoldPoints = l_me->m_Draw3D.pOldPoints;
				l_me->m_Draw3D.pOldPoints = l_me->m_Draw3D.pNewPoints;
				l_me->m_Draw3D.numVoldPoints = l_me->m_Draw3D.numOldPoints;
				l_me->m_Draw3D.numOldPoints = l_me->m_Draw3D.numNewPoints;
				delete l_me->m_Draw3D.points3D;
				
				/// calcul des nouveaux points
				/// On change les angles de rotation, en fonction du temps
				/// qui s'est écoulé depuis la dernière image
				bigtime_t newTime = system_time();
				float rotateTime = (float)(newTime-l_me->m_LastTime)/2000000.0;
				l_me->m_LastTime = newTime;
				l_me->m_XAngle+=rotateTime*l_me->m_XSpeed;
				l_me->m_YAngle+=rotateTime*l_me->m_YSpeed;
				
				/// mise en place des nouveaux paramètres: couleurs et nombre
				/// de points (au cas où ça a changé depuis la dernière fois)
				l_me->m_Draw3D.numNewPoints = l_me->m_Points->NumPoints();
				l_me->m_Draw3D.pointsColor = l_me->m_Points->Colors();
			
				/// rotation des points: calcul de la matrice
				MTMatrix rot(l_me->m_BaseMatrix);
				rot *= MTMatrix::RotateY(l_me->m_YAngle);
				rot *= MTMatrix::RotateX(l_me->m_XAngle);
				rot *= MTMatrix::Scale(l_me->m_Draw3D.objectSize*0.75);
				
				/// rotation des points: application de la transformation
				MTPoints *newPoints = new MTPoints(*(l_me->m_Points));
				*newPoints *= rot;
				l_me->m_Draw3D.points3D = newPoints;

				/// projection 3D -> 2D
				l_me->m_Draw3D.pNewPoints = new BPoint[newPoints->NumPoints()];
				newPoints->Project(-1000,l_me->m_Draw3D.pNewPoints);
				
				/// déverouillage de l'accès aux données 3D
				l_me->m_3DDataLock.Unlock();
			}
		}

		// on peut débloquer
		if (bloque)
		{
			l_me->m_Locker.Unlock();
			bloque = false;
		}
	
		// pause... uniquement en double buffering (en simple, il y a le
		// WaitForRetrace qui fait la pause)
		if (l_me->m_Draw3D.softwareDouble)	
			snooze(16000);
		
		// si le sémaphore bloque une mise à jour des points, on le libère
		int32 l_count;
		get_sem_count(l_me->m_ChangeSema,&l_count);
		if (l_count<=0)
			release_sem(l_me->m_ChangeSema);	
	}
	return B_OK;
}

void ViewWindow::NewPoints(MTPoints *newPoints)
{
	if (newPoints == NULL)
		return;

	// cette partie n'est pas parfaite, il y a sans doute un problème de
	// synchro avec le thread de dessin. Quand on fait un morphing très actif
	// (c'est-à-dire avec beaucoup d'appels de NewPoints()) les couleurs
	// sautent. Le sémaphore arange les choses, mais normalement on ne devrait
	// pas en avoir besoin	
	acquire_sem(m_ChangeSema);
	m_3DDataLock.Lock();
	MTPoints *l_OldPoints = m_Points;
	m_Points = newPoints;
	delete l_OldPoints;
	m_3DDataLock.Unlock();
	
	BString message;
	message << newPoints->NumPoints() << " points";
	Lock();
	m_StringView->SetText(message.String());
	Unlock();
}

TDMView::TDMView(BRect frame, ViewWindow *window)
: BView(frame,NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW), m_Window(window)
{
	SetViewColor(B_TRANSPARENT_32_BIT);
	
	rgb_color noir = { 0,0,0 };
	SetHighColor(noir);
	
	m_Clic = false;
}

TDMView::~TDMView()
{
}

void TDMView::MouseDown(BPoint point)
{
	SetMouseEventMask(B_POINTER_EVENTS);
	
	// on arrête la rotation en cours
	m_Window->m_XSpeed = 0;
	m_Window->m_YSpeed = 0;
	
	// on applique la transformation actuelle pour repartir de zéro
	m_Window->m_3DDataLock.Lock();
	m_Window->m_BaseMatrix *= MTMatrix::RotateY(m_Window->m_YAngle);
	m_Window->m_BaseMatrix *= MTMatrix::RotateX(m_Window->m_XAngle);
	m_Window->m_YAngle = m_Window->m_XAngle = 0;
	m_Window->m_3DDataLock.Unlock();
	
	// on note les coordonnées actuelles de clic de souris, ça servira
	// de repère quand on la bougera
	m_OldX = m_VOldX = point.x;
	m_OldY = m_VOldY = point.y;
	m_OldTime = system_time();

	m_Clic = true;
}

void TDMView::MouseMoved(BPoint point,uint32,const BMessage*)
{
	if (m_Clic)
	{
		m_Window->m_XAngle += (point.y-m_OldY)/m_Window->m_Draw3D.objectSize;	
		m_Window->m_YAngle += (point.x-m_OldX)/m_Window->m_Draw3D.objectSize;	

		m_VOldX = m_OldX;
		m_VOldY = m_OldY;
		m_OldX = point.x;
		m_OldY = point.y;
		m_VOldTime = m_OldTime;
		m_OldTime = system_time();
	}
}

void TDMView::MouseUp(BPoint point)
{
	// on regarde combien de temps s'est écoulé depuis le dernier mouvement
	bigtime_t l_time = system_time()-m_VOldTime;
	if (l_time>500000)
	{
		// ça fait une demi-seconde, ça veut dire qu'on veut l'arréter
		m_Window->m_XSpeed = 0;
		m_Window->m_YSpeed = 0;
	}
	else
	{
		m_Window->m_XSpeed = (point.y-m_VOldY)/m_Window->m_Draw3D.objectSize*
			1000000/l_time;
		m_Window->m_YSpeed = (point.x-m_VOldX)/m_Window->m_Draw3D.objectSize*
			1000000/l_time;
	}
		
	m_Clic = false;
}
