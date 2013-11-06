/*
 * 3D Morph Toolkit
 *
 * fenêtre principale
 *
**/

#include "Mainwin.h"
#include <View.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Application.h>
#include <Button.h>
#include <Slider.h>
#include <StringView.h>
#include <FilePanel.h>
#include <File.h>
#include <DataIO.h>
#include <NodeInfo.h>
#include <Alert.h>
#include <Application.h>
#include <stdio.h>
#include <stdlib.h>
#include <Path.h>
#include <Directory.h>
#include <String.h>

#include "MTObjectTrans.h"
#include "MTTextObject.h"
#include "MTImageObject.h"
#include "ViewWindow.h"
#include "MTPoints.h"
#include "MTAddOn.h"
#include "3DMTApp.h"
#include "Preferences.h"
#include "HappyCommander.h"

// à virer
#include <stdio.h>

unsigned int s_MWWidth = 350;	// taille initiale (et minimale) de la fenêtre
unsigned int s_MWMargin = 10;	// marges à utiliser entre les éléments;
unsigned int s_MWButtonWidth = 80;	// taille du bouton
unsigned int s_MWLabelSize = 50;	// taille du bout de texte, quand le
									//  slider 'morphing' est visible

#define SMW_LOAD 'MwLo'
#define SMW_LOAD2 'MwL2'
#define SMW_LOAD_FILE 'MlFi'
#define SMW_SAVE 'MwSv'
#define SMW_SAVE_FILE 'MwSf'
#define SMW_OPEN 'MwOp'
#define SMW_UPDATE 'MwUp'
#define SMW_BUFFERING 'MwBu'
#define SMW_HELP 'MwHl'
#define SMW_HELP_APPLICATION 'MwHA'
#define SMW_HELP_ADDON 'MwHD'
#define SMW_HELP_INCLUDED 'MwHI'
#define SMW_MORPHING 'MwMo'

MainWin *g_MainWin;	// pointeur vers la fenêtre principale
BFilePanel *g_FilePanel;	// pointeur vers le panel utilisé pour charger des fichiers

MainWin::MainWin(BRect frame)
: BWindow(BRect(frame.left,frame.top,frame.left+s_MWWidth,frame.top+30),
	"3D Morph Toolkit",B_TITLED_WINDOW,
	B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_ASYNCHRONOUS_CONTROLS)
{
	g_MainWin = this;
	
	g_Preferences->SizeWindow("Main Window",this);

	// gestion de BeHappy
	m_BeHappy = new HappyCommander("3DM Toolkit Documentation","BH3DMT",
		"Doc/index.html");
	int l_DoInstall = g_Preferences->FindInt(1,"Do BHA install");
	if (l_DoInstall != HappyCommander::HC_NO_MORE_ASKING)
		l_DoInstall = m_BeHappy->InstallAddOn();
	
	g_Preferences->AddInt(l_DoInstall,"Do BHA install");
	
	// vue principale
	m_Viewer = NULL;
	m_MainView = new BView(Bounds(),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	m_MainView->SetViewColor(216,216,216);
	AddChild(m_MainView);
	
	// installation du menu
	BMenuBar *l_MenuBar = new BMenuBar(Bounds(),NULL);
	
	// menu fichier
	BMenu *l_FileMenu = new BMenu("File");
	l_FileMenu->AddItem(new BMenuItem("Load Object",new BMessage(SMW_LOAD),'L'));
	m_MILoad2 = new BMenuItem("Load Object n°2",new BMessage(SMW_LOAD2),'L',
		B_SHIFT_KEY);
	l_FileMenu->AddItem(m_MILoad2);
	m_MILoad2->SetEnabled(false);
	m_MISave = new BMenuItem("Save result",new BMessage(SMW_SAVE),'S');
	l_FileMenu->AddItem(m_MISave);
	m_MISave->SetEnabled(false);
	l_FileMenu->AddSeparatorItem();
	l_FileMenu->AddItem(new BMenuItem("About 3DMorph ToolKit",
		new BMessage(B_ABOUT_REQUESTED),'B'));
	l_FileMenu->AddSeparatorItem();
	l_FileMenu->AddItem(new BMenuItem("Quit",new BMessage(B_QUIT_REQUESTED),'Q'));
	l_MenuBar->AddItem(l_FileMenu);
	
	// menu view
	BMenu *l_ViewMenu = new BMenu("View");
	l_ViewMenu->AddItem(new BMenuItem("Open 3D Window",new BMessage(SMW_OPEN),'O'));
	m_MIUpdate = new BMenuItem("Update view",new BMessage(SMW_UPDATE),'U');
	l_ViewMenu->AddItem(m_MIUpdate);
	m_MIUpdate->SetEnabled(false);
	l_MenuBar->AddItem(l_ViewMenu);
	
	// menu d'aide
	BMenu *l_HelpMenu = new BMenu("Help");
	l_HelpMenu->AddItem(new BMenuItem("All",new BMessage(SMW_HELP),'H'));
	l_HelpMenu->AddSeparatorItem();
	/// le reste du menu dépend de l'installation de l'add-on pour BeHappy
	if (l_DoInstall == HappyCommander::HC_OK || 
		l_DoInstall == HappyCommander::HC_ALREADY_INSTALLED)
	{
		l_HelpMenu->AddItem(new BMenuItem("This Application",new BMessage(SMW_HELP_APPLICATION)));
		l_HelpMenu->AddItem(new BMenuItem("Using bitmaps",new BMessage(SMW_HELP_INCLUDED)));
		l_HelpMenu->AddItem(new BMenuItem("Writing an add-on",new BMessage(SMW_HELP_ADDON)));
	}
	else
	{
		BMenuItem *l_Item = new BMenuItem("No BeHappy...",NULL);
		l_Item->SetEnabled(false);
		l_HelpMenu->AddItem(l_Item);
	}
	l_MenuBar->AddItem(l_HelpMenu);
	
	// ajout du menu dans la fenêtre
	m_MainView->AddChild(l_MenuBar);
	
	// ajout du bouton et autres gadgets
	/// création du rectangle pour placer les vues
	BRect l_Rect(s_MWMargin,l_MenuBar->Bounds().bottom+s_MWMargin,
		s_MWMargin+s_MWButtonWidth,l_MenuBar->Bounds().bottom+s_MWMargin);
	
	/// bouton 'update'
	m_Button = new BButton(l_Rect,NULL,"Update",new BMessage(SMW_UPDATE));
	m_Button->SetEnabled(false);
	m_MainView->AddChild(m_Button);
	l_Rect = m_Button->Frame();	// pour récupérer la taille verticale
	
	/// label
	l_Rect.left = l_Rect.right + s_MWMargin;
	l_Rect.right = s_MWWidth - s_MWMargin;
	m_Label = new BStringView(l_Rect,NULL,
		"Use the menu to load objects",
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	m_MainView->AddChild(m_Label);
	
	/// slider pour les morphings
	l_Rect.left += s_MWLabelSize + s_MWMargin;
	m_Morphing = new BSlider(l_Rect,NULL,NULL,NULL,0,1000,B_BLOCK_THUMB,
		B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP);
	m_Morphing->SetModificationMessage(new BMessage(SMW_MORPHING));
	m_MainView->AddChild(m_Morphing);
	m_Morphing->Hide();
	
	/// redimensionnement de la fenêtre
	m_ToolbarHeight = l_Rect.bottom+s_MWMargin;
	ResizeTo(s_MWWidth,m_ToolbarHeight);
	
	// et on affiche le tout
	Show();
	
	// initialisation de quelques variables
	m_Object1 = m_Object2 = NULL;
	m_Object1View = m_Object2View = NULL;
	m_Points1 = m_Points2 = NULL;
	m_AddOnID1 = m_AddOnID2 = -1;
	m_SavePanel = NULL;

	// création du panel pour charger des trucs
	entry_ref *l_UserPathRef = NULL;
	BEntry l_UserPathEntry;
	
	/// on récupère le chemin stoqué dans les préférences
	BString *l_UserPathString = g_Preferences->FindString("","FilePanelPath");
	if ((l_UserPathString->Length() > 0) && 
		(l_UserPathEntry.SetTo(l_UserPathString->String()) == B_OK))
	{
		l_UserPathRef = new entry_ref;
		l_UserPathEntry.GetRef(l_UserPathRef);
	}
	
	g_FilePanel = new BFilePanel(B_OPEN_PANEL,NULL,l_UserPathRef,0,false);
}

MainWin::~MainWin()
{
	g_Preferences->CloseWindow(this);

	// on récupère le chemin du sélecteur de fichiers
	{
		BEntry l_UserPathEntry;
		entry_ref l_UserPathRef;
		
		g_FilePanel->GetPanelDirectory(&l_UserPathRef);
		l_UserPathEntry.SetTo(&l_UserPathRef);
		
		BPath l_UserPath;
		l_UserPathEntry.GetPath(&l_UserPath);
		
		g_Preferences->AddString(l_UserPath.Path(),"FilePanelPath");
	}
	
	delete g_FilePanel;
	g_FilePanel = NULL;
	delete m_SavePanel;
}

void MainWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		// on veut charger un fichier pour l'objet du haut
		case SMW_LOAD:
		{
			BMessage l_Notification(SMW_LOAD_FILE);
			l_Notification.AddInt16("Number",1);
			g_FilePanel->SetMessage(&l_Notification);
			
			BMessenger l_Me(this);
			g_FilePanel->SetTarget(l_Me);
			
			g_FilePanel->Show();
		}
		break;
	
		// on veut charger un fichier pour l'objet du bas
		case SMW_LOAD2:
		{
			BMessage l_Notification(SMW_LOAD_FILE);
			l_Notification.AddInt16("Number",2);
			g_FilePanel->SetMessage(&l_Notification);
			
			BMessenger l_Me(this);
			g_FilePanel->SetTarget(l_Me);
			
			g_FilePanel->Show();
		}
		break;
	
		// on veut sauver l'objet
		case SMW_SAVE:
		{
			// si le panel pour sauver n'existe pas, on le crée
			if (m_SavePanel == NULL)
			{
				entry_ref l_curDir;
				g_FilePanel->GetPanelDirectory(&l_curDir);
				m_SavePanel = new BFilePanel(B_SAVE_PANEL,
					new BMessenger(this),&l_curDir,0,false,
					new BMessage(SMW_SAVE_FILE));
			}
			
			m_SavePanel->Show();
		}
		break;
		
		// l'utilisateur a sélectionné un fichier. On va l'ouvrir
		case SMW_LOAD_FILE:
		{
			int16 l_Num;
			if ((message->FindInt16("Number",&l_Num) == B_OK) &&
				(l_Num>=1) && (l_Num<=2))
			{
				entry_ref l_EntryRef;
				if (message->FindRef("refs",&l_EntryRef) == B_OK)
					LoadFile(l_Num,l_EntryRef);
			}
		}
		break;
		
		// l'utilisateur veut qu'on ouvre la fenêtre 3D
		case SMW_OPEN:
		{
			// si elle est déjà ouverte, on ne fait rien
			if (m_Viewer == NULL)
			{
				m_Viewer = new ViewWindow(BRect(200,200,400,400));
				UpdateViewWindow();
				m_MISave->SetEnabled(true);
				m_MIUpdate->SetEnabled(true);
				m_Button->SetEnabled(true);
			}
		}
		break;
		
		// l'utilisateur a donné le nom du fichier que l'on va utiliser
		case SMW_SAVE_FILE:
		{
			if (m_Viewer == NULL)
				(new BAlert("3DMorph Toolkit","Nothing to save!","Cancel",
					NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			else
			{
				entry_ref l_SaveDir;
				const char *l_SaveName;
				if ((message->FindRef("directory",&l_SaveDir) == B_OK) &&
					(message->FindString("name",&l_SaveName) == B_OK))
					SaveFile(l_SaveDir,l_SaveName);
			}
		}
		break;
		
		// mise à jour 3D
		case SMW_UPDATE:
			CalcObjects();
		case SMW_MORPHING:
			UpdateViewWindow();
			break;
		
		// à propos....
		case B_ABOUT_REQUESTED:
		{
			BAlert *infoAlert = new BAlert("3DMorph Toolkit",
				"3D Morph Toolkit\nBy Sylvain Tertois\n","More... (help)",
				"OK",NULL,B_WIDTH_AS_USUAL,B_INFO_ALERT);
			if (infoAlert->Go() == 0)
				PostMessage(SMW_HELP);
		}
		break;
		
		// aide
		case SMW_HELP:
			m_BeHappy->Show();
		break;

		case SMW_HELP_APPLICATION:
			m_BeHappy->Show("Summary","Using the application");
		break;
		
		case SMW_HELP_ADDON:
			m_BeHappy->Show("Summary","Doing an add-on");
		break;
				
		case SMW_HELP_INCLUDED:
			m_BeHappy->Show("Summary","Loading Bitmaps");
		break;
		
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

bool MainWin::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	
	if (m_Viewer != NULL)
	{
		// attente de la fin du thread
		thread_id l_TId = m_Viewer->Thread();
		m_Viewer->PostMessage(B_QUIT_REQUESTED);
		status_t l_Dummy;
		wait_for_thread(l_TId,&l_Dummy);
	}
		
	return true;
}

void MainWin::RequestUpdate()
{
	PostMessage(SMW_UPDATE);
}

// chargement d'un fichier comme objet pour le toolkit
void MainWin::LoadFile(int16 num, entry_ref &ref)
{
	// nouvel objet
	MTObject *l_NewObject;
	
	// pointeur vers l'objet actuel
	MTObject **l_Object = (num==1?&m_Object1:&m_Object2);
	
	// si c'est un add-on, on aura besoin de ces variables:
	image_id *l_AddOnID = (num==1?&m_AddOnID1:&m_AddOnID2);	
		// pointeur vers l'ID de l'add-on de l'objet
	image_id l_NewAddOn = -1;		// ID du nouvel add-on
	
	// alors, c'est quoi ce fichier?
	BFile l_File(&ref,B_READ_ONLY);
	if (l_File.InitCheck() != B_OK)
		return;
	
	// maintenant on est sûr que le fichier existe
	// récupération de son type MIME
	BNodeInfo l_FileInfo(&l_File);
	char l_MimeType[B_MIME_TYPE_LENGTH+1];
	if (l_FileInfo.GetType(l_MimeType) != B_OK)
		return;
	
	// cas n°1: il s'agit d'un add-on
	if (BString(l_MimeType) == "application/x-vnd.Be-elfexecutable")
	{
		/// chargement de l'add-on
		BPath l_AddOnPath(&ref);
		l_NewAddOn = load_add_on(l_AddOnPath.Path());
		if (l_NewAddOn<0)
		{
			(new BAlert("3DMorph Toolkit","Unable to load add-on","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			return;
		}
		
		/// on récupère le type de l'add-on et la fonction d'instanciation
		MTAddOnType *l_AddOnType;
		MTObject* (*l_InstFunction)();
		if ((get_image_symbol(l_NewAddOn,"addOnType",B_SYMBOL_TYPE_DATA,
				(void**)&l_AddOnType) != B_OK) ||
			(get_image_symbol(l_NewAddOn,"InstantiateObject",
				B_SYMBOL_TYPE_TEXT,(void**)&l_InstFunction) != B_OK))
		{
			/// il manque quelque chose
			(new BAlert("3DMorph Toolkit","Invalid add-on","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			unload_add_on(l_NewAddOn);
			return;
		}
		
		/// on vérifie que le type d'add-on est bon		
		if ((*l_AddOnType & MT_TOOLKIT) == 0)
		{
			/// pas compatible
			(new BAlert("3DMorph Toolkit","Uncompatible add-on","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			unload_add_on(l_NewAddOn);
			return;
		}
		
		/// on crée l'objet
		l_NewObject = (*l_InstFunction)();
		if (l_NewObject == NULL)
		{
			/// marche pas
			(new BAlert("3DMorph Toolkit","Add-on error","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			unload_add_on(l_NewAddOn);
			return;
		}
		 			
		/// c'est bon, c'est créé.
	}
	else // l_MimeType != "application/x-vnd.Be-elfexecutable"
	{
		// cas n°2: c'est autre chose	
		// chargement des données
		off_t l_Size;
		if ((l_File.GetSize(&l_Size) != B_OK) || (l_Size <= 0))
			return;
		char *l_Buffer = new char[l_Size];
		if (l_File.Read((void*)l_Buffer,l_Size) != l_Size)
		{
			(new BAlert("3DMorph Toolkit","Unable to load data","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			delete l_Buffer;
			return;
		}
		
		// l'objet en cours est-il un 'translator'?
		MTObjectTranslator *l_Translator;
		try { l_Translator = dynamic_cast<MTObjectTranslator*>(*l_Object); }
		catch (...) { l_Translator = NULL; }
		if (l_Translator != NULL)
		{
			// est-ce qu'il gère le fichier en question?
			if (l_Translator->CanHandleType(l_MimeType))
			{
				// oui... on y va!
				l_Translator->Load(l_MimeType,(void*)l_Buffer,l_Size);
				UpdateViewWindow();

				delete l_Buffer;
				return;
			}
		}
	
		// bon... soit ce n'est pas un translator, soit il ne gère pas ce 
		// fichier
		// on cherche l'objet à mettre
		// on teste avec un MTTexObject
		l_NewObject = new MTTextObject;
		if (!((MTObjectTranslator*)l_NewObject)->CanHandleType(l_MimeType))
		{
			// marche pas... on teste avec un MTImageObject
			delete l_NewObject;
			l_NewObject = new MTImageObject;
			if (!((MTObjectTranslator*)l_NewObject)->CanHandleType(l_MimeType))
			{
				// pas trouvé du tout...
				delete l_NewObject;
				l_NewObject = NULL;
				(new BAlert("3DMorph Toolkit","I don't know what to do with this",
					"Cancel",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
				delete l_Buffer;
				return;
			}
		}
		// on a trouvé... on donne les données au translator
		((MTObjectTranslator*)l_NewObject)->Load(l_MimeType,(void*)l_Buffer,l_Size);
	
	} // if(l_MimeType == "application/x-vnd.Be-elfexecutable")
	
	// le nouvel objet est dans l_NewObject
	// on va l'installer
	InstallObject(num,l_NewObject);

	// si il y avait un add-on avant, il faut le virer
	if (*l_AddOnID >= 0)
		unload_add_on(*l_AddOnID);
	
	// et on place le nouvel Add-on. (si l'objet est un translator integré,
	// l_NewAddOn vaut -1, et donc *l_AddOnID va aussi valoir -1, et on 
	// saura que ça ne correspond pas à un add-on.
	*l_AddOnID = l_NewAddOn;
}

// installation d'un nouvel objet
void MainWin::InstallObject(int16 num, MTObject *newObject)
{
	// quelques variables
	float l_WinWidth,l_WinHeight;	// future taille de la fenêtre
	
	// Réglage de l'invoqueur de l'objet pour une mise à jour
	newObject->m_UpdateInvoker.SetMessage(new BMessage(SMW_UPDATE));
	newObject->m_UpdateInvoker.SetTarget(this);
		
	// quel objet devra t'on changer?
	BView **l_ObjectView = (num==1?&m_Object1View:&m_Object2View);
	MTObject **l_Object = (num==1?&m_Object1:&m_Object2);
	
	// on va gentilment virer l'ancien objet
	if (*l_Object != NULL)
	{
		m_MainView->RemoveChild(*l_ObjectView);
		delete *l_ObjectView;
		*l_ObjectView = NULL;
	}
	delete *l_Object;

	// et prendre le nouveau
	*l_Object = newObject;
	*l_ObjectView = (*l_Object)->CreateView();
	
	// si la vue est NULL, on en crée une fausse à la place
	if (*l_ObjectView == NULL)
		*l_ObjectView = new BStringView(BRect(0,0,350,16),NULL,
			"This object hasn't any configurable parameters",
			B_FOLLOW_LEFT_RIGHT);
	
	// positionnement de la vue
	m_Object1View->MoveTo(0,m_ToolbarHeight);
	if (m_Object2 != NULL)
		m_Object2View->MoveTo(0,m_Object1View->Frame().bottom+1);

	// redimensionnement de la fenêtre
	l_WinWidth = s_MWWidth;
	if (m_Object1View->Bounds().Width() > l_WinWidth)
		l_WinWidth = m_Object1View->Bounds().Width();
	if ((m_Object2 != NULL) && (m_Object2View->Bounds().Width() > l_WinWidth))
		l_WinWidth = m_Object2View->Bounds().Width();

	l_WinHeight = m_Object1View->Frame().bottom;
	if (m_Object2 != NULL)
		l_WinHeight += m_Object2View->Frame().Height();
	
	ResizeTo(l_WinWidth,l_WinHeight);
	
	// et plaçage de la vue dans la fenêtre
	m_MainView->AddChild(*l_ObjectView);
	
	// changement de quelques éléments de menu et de l'interface
	m_MILoad2->SetEnabled(true);
	if (num==2)
	{
		m_Label->SetText("Morphing:");
		m_Label->ResizeTo(s_MWLabelSize,m_Label->Frame().Height());
		m_Morphing->Show();
	}
	CalcObjects();
	UpdateViewWindow();
}

// met à jour la fenêtre 3D
void MainWin::UpdateViewWindow()
{
	if (m_Viewer == NULL)
		return;
		
	MTPoints *points = NULL;
	
	// faut-il faire un morphing?
	if ((m_Object1 != NULL) && (m_Object2 != NULL) && (m_Points1 != NULL) &&
		(m_Points2 != NULL))
	{
		float f = m_Morphing->Position();
		points = new MTPoints(MTPoints::Morphing(*m_Points1,*m_Points2,f));
	}
	else
		if ((m_Object2 == NULL) && (m_Points1 != NULL))
			points = new MTPoints(*m_Points1);
		else if (m_Points2 != NULL)
			points = new MTPoints(*m_Points2);
		
	if (points != NULL)
		m_Viewer->NewPoints(points);
}

// fait une mise à jour des points
void MainWin::CalcObjects()
{
	if (m_Object1 != NULL)
	{
		delete m_Points1;
		m_Points1 = m_Object1->Update(0);
	}
	if (m_Object2 != NULL)
	{
		delete m_Points2;
		m_Points2 = m_Object2->Update(0);
	}
}

void MainWin::SaveFile(entry_ref &directory, const char *name)
{
	MTPoints *l_Points = m_Viewer->m_Points;
	if ((l_Points == NULL) || (l_Points->NumPoints() == 0))
		(new BAlert("3D Morph Toolkit","Nothing to save!","Cancel",NULL,NULL,
			B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
	else
	{
		// extraction du nom du fichier
		BDirectory l_DestDir(&directory);
		BPath l_FilePath(&l_DestDir,name);
		FILE *l_OutFile = fopen(l_FilePath.Path(),"w");
		if (l_OutFile == NULL)
			(new BAlert("3D Morph Toolkit","Unable to create file!","Cancel",
				NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
		else
		{
			// ligne d'intro et nombre de points
			fprintf(l_OutFile,"File generated with the 3DMorph Toolkit, by Sylvain Tertois.\n");
			fprintf(l_OutFile,"#N%d\n",l_Points->NumPoints());
			
			// on récupère les infos utiles
			const float *l_Coords = l_Points->Coordinates();
			const rgb_color *l_Colors = l_Points->Colors();
			for (unsigned int i=0; i<l_Points->NumPoints(); i++)
			{
				fprintf(l_OutFile,"#P%1.4f %1.4f %1.4f %1.4f %1.4f %1.4f\n",
					l_Coords[0],l_Coords[1],l_Coords[2],
					(float)(l_Colors->red/255.0),
					(float)(l_Colors->green/255.0),
					(float)(l_Colors->blue/255.0));
				l_Coords+= 3;
				l_Colors++;
			}
			
			fclose(l_OutFile);
		}
	}
}
