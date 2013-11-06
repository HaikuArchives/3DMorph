// Configuration du dessin

#include <InterfaceKit.h>
#include "ScreenConfig.h"
#include "morphR45.h"
#include <stdlib.h>

static const char *tsr[6] = { "640*480 ", "800*600 ", "1024*768", "1152*900", "1280*1024", "1600*1200" };
static const char *tsd[3] = { "8 bits", "16 bits", "32 bits" };
//static const char *tsp[5] = { "very low", "low", "normal", "high", "very high" };

//// Fonctions utilitaires pour obtenir une copie des textes ci-dessus
// Fonctions utilitaires pour obtenir un des textes ci-dessus
static const char *GetResolutionText(unsigned int i)
{
/*	char *s = new char[strlen(tsr[i])+1];
	strcpy(s,tsr[i]);
	return s;
*/	return tsr[i];
}

static const char *GetDepthText(unsigned int i)
{
/*	char *s = new char[strlen(tsd[i])+1];
	strcpy(s,tsd[i]);
	return s;
*/	return tsd[i];
}

/*static const char *GetPriorityText(unsigned int i)
{
	return tsp[i];
}
*/
 
ScreenConfig::ScreenConfig(BRect r,MorphSaver *ms)
	: BView(r,NULL,B_FOLLOW_ALL,B_WILL_DRAW)
{
	saver = ms;
	SetViewColor(216,216,216,0);

	BRect frame = Bounds();
	float myLeft = frame.left, myRight = frame.right;
	float myMiddle = (myLeft+myRight)/2;
	
	frame.bottom = frame.top + 16;
	
	// création des deux checkbox
	frame.right = myMiddle;
	cbDoubleBuf = new BCheckBox(frame,NULL,"Double buffering",new BMessage('UPDT'));
	AddChild(cbDoubleBuf);
	
	frame.left = myMiddle;
	frame.right = myRight;
	cbSoft = new BCheckBox(frame,NULL,"Software",new BMessage('UPDT'));
	AddChild(cbSoft);
	
	frame.top += 24.0;
	frame.bottom = frame.top + 16;
	frame.left = myLeft;
	cbWait = new BCheckBox(frame,B_EMPTY_STRING,"Wait for retrace",new BMessage('UPDT'));
	AddChild(cbWait);
	
	// création des deux menus pour choisir le type d'affichage
	/// résolution
	BMenu *menu = new BMenu("");
	frame.top += 24.0;
	frame.bottom = frame.top + 16;
	mfResolution = new BMenuField(frame,"Resolution menu","Resolution",menu);
	AddChild(mfResolution);
	
	/// couleurs
	menu = new BMenu("");
	frame.top += 24.0;
	frame.bottom = frame.top + 16;
	mfDepth = new BMenuField(frame,"Depth menu","Color depth",menu);
	AddChild(mfDepth);

	// création de la checkbox pour la qualité des points
	frame.top += 24.0;
	frame.bottom = frame.top + 16;
	cbNice = new BCheckBox(frame,B_EMPTY_STRING,"Nicer Points",new BMessage('UPDT'));
	AddChild(cbNice);
	
	// et du menu pour la priorité
//	menu = new BMenu("");
//	frame.top += 24.0;
//	frame.bottom = frame.top + 16;
//	mfPriority = new BMenuField(frame,"Priority menu","Priority",menu);
//	AddChild(mfPriority);
	
	// on règle les valeurs des checkbox
	cbDoubleBuf->SetValue(saver->draw.doubleBuffering);
	cbWait->SetValue(saver->waitForRetrace);
	cbSoft->SetValue(saver->draw.softwareDouble);
	cbNice->SetValue(saver->draw.m_NiceDraw);
	selRes = GetResolution(saver->screenSpace);
	switch(GetBitsPPixel(saver->screenSpace))
	{
		case 8:
			selBits = 0;
			break;
		
		case 15:
		case 16:
			selBits = 1;
			break;
			
		case 32:
			selBits = 2;
			break;
		
		default:
			selBits = 0;
			break;
	}
//	selPriority = saver->m_PriorityNum;

	attachedDone = false;
	DoUpdate();
}

ScreenConfig::~ScreenConfig()
{
	mfResolution->RemoveSelf();
	delete mfResolution;
	
	mfDepth->RemoveSelf();
//	delete mfDepth;
// si je mets le delete, ça plante à la fermeture de la fenêtre de config...

//	mfPriority->RemoveSelf();
//	delete mfPriority;
}

void ScreenConfig::SaveState()
{
	// il faut renvoyer au screensaver la config contenue dans l'interface
	saver->draw.doubleBuffering = cbDoubleBuf->Value();
	saver->draw.softwareDouble = cbSoft->Value();
	saver->draw.m_NiceDraw = cbNice->Value();
	
	if (!saver->draw.doubleBuffering)
		saver->waitForRetrace = cbWait->Value();
	
	// on convertit les deux valeurs résolution et couleurs en espace
	unsigned int bits;
	switch(selBits)
	{
		case 0:
			bits = 8;
			break;
		
		case 1:
			bits = 16;
			break;
		
		case 2:
			bits = 32;
			break;
		
		default:
			bits = 8;
			break;
	}
	saver->screenSpace = GetSpace(selRes,bits);		
//	saver->m_PriorityNum = selPriority;
}

void ScreenConfig::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		// on a sélectionné une nouvelle résolution
		case 'NRes':
			message->FindInt32("num",(int32*)&selRes);
			DoUpdate();
			break;
		
		// on a sélectionné une nouvelle profondeur de couleurs
		case 'NDep':
			message->FindInt32("num",(int32*)&selBits);
			DoUpdate();
			break;
		
		// on a sélectionné une nouvelle priorité
//		case 'NPri':
//			message->FindInt32("num",(int32*)&selPriority);
//			DoUpdate();
//			break;
			
		// on demande une mise à jour			
		case 'UPDT':
			DoUpdate();
			break;
			
		default:
			BHandler::MessageReceived(message);
	}
}

void ScreenConfig::DoUpdate()
{
	// mise à jour des différents champs, et notamment réglage des activations des vues
	if (Window() != NULL)
		Window()->Lock();

	if (cbDoubleBuf->Value() != 0)
	{
		cbWait->SetValue(1);
		cbWait->SetEnabled(false);
		cbSoft->SetEnabled(true);
		
		if (cbSoft->Value() != 0)
		{
			mfResolution->SetEnabled(false);
			mfDepth->SetEnabled(false);
		}
		else
		{
			mfResolution->SetEnabled(true);
			mfDepth->SetEnabled(true);
		}
	}
	else
	{
		mfResolution->SetEnabled(false);
		mfDepth->SetEnabled(false);
		cbWait->SetEnabled(true);
		cbSoft->SetEnabled(false);
	}	

	mfResolution->Menu()->Superitem()->SetLabel(GetResolutionText(selRes));
	mfDepth->Menu()->Superitem()->SetLabel(GetDepthText(selBits));
//	mfPriority->Menu()->Superitem()->SetLabel(GetPriorityText(selPriority));

	if (Window() != NULL)
		Window()->Unlock();
}

void ScreenConfig::AttachedToWindow()
{
	// quand on est attachés à la fenêtre, il faut dire aux contrôles de nous envoyer un message
	// à nous et pas à la fenêtre
	// et on remplit également les menus
	if (attachedDone)
		return;
	
	// résolutions	
	for (unsigned int i=0; i<6; i++)
	{
		BMessage *newMessage = new BMessage('NRes');
		newMessage->AddInt32("num",i);
		BMenuItem *item = new BMenuItem(GetResolutionText(i),newMessage);
		item->SetTarget(BMessenger(this));
		mfResolution->Menu()->AddItem(item);
	} 
	
	// couleurs
	for (unsigned int i=0; i<3; i++)
	{
		BMessage *newMessage = new BMessage('NDep');
		newMessage->AddInt32("num",i);
		BMenuItem *item = new BMenuItem(GetDepthText(i),newMessage);
		item->SetTarget(BMessenger(this));
		mfDepth->Menu()->AddItem(item);
	}
	
	// priorités
//	for (unsigned int i=0; i<5;i++)
//	{
//		BMessage *newMessage = new BMessage('NPri');
//		newMessage->AddInt32("num",i);
//		BMenuItem *item = new BMenuItem(GetPriorityText(i),newMessage);
//		item->SetTarget(BMessenger(this));
//		mfPriority->Menu()->AddItem(item);
//	}
	
	// réglage des cibles
	cbDoubleBuf->SetTarget(BMessenger(this));
	cbWait->SetTarget(BMessenger(this));
	cbSoft->SetTarget(BMessenger(this));
	cbNice->SetTarget(BMessenger(this));

	attachedDone = true;
}

ScreenResolution ScreenConfig::GetResolution(uint32 space)
{
	switch(space)
	{
		case B_8_BIT_640x480:
		case B_16_BIT_640x480:
		case B_32_BIT_640x480:
			return S_640x480;
			break;
			
		case B_8_BIT_800x600:
		case B_16_BIT_800x600:
		case B_32_BIT_800x600:
			return S_800x600;
			break;

		case B_8_BIT_1024x768:
		case B_16_BIT_1024x768:
		case B_32_BIT_1024x768:
			return S_1024x768;
			break;

		case B_8_BIT_1152x900:
		case B_16_BIT_1152x900:
		case B_32_BIT_1152x900:
			return S_1152x900;
			break;

		case B_8_BIT_1280x1024:
		case B_16_BIT_1280x1024:
		case B_32_BIT_1280x1024:
			return S_1280x1024;
			break;

		case B_8_BIT_1600x1200:
		case B_16_BIT_1600x1200:
		case B_32_BIT_1600x1200:
			return S_1600x1200;
			break;

		default:
			return S_640x480;		// juste pour être sûr, on rend le + petit
	}
}

unsigned int ScreenConfig::GetBitsPPixel(uint32 space)
{
	switch(space)
	{
		case B_8_BIT_640x480:
		case B_8_BIT_800x600:
		case B_8_BIT_1024x768:
		case B_8_BIT_1152x900:
		case B_8_BIT_1280x1024:
		case B_8_BIT_1600x1200:
			return 8;
			break;
		
		case B_16_BIT_640x480:
		case B_16_BIT_800x600:
		case B_16_BIT_1024x768:
		case B_16_BIT_1152x900:
		case B_16_BIT_1280x1024:
		case B_16_BIT_1600x1200:
			return 16;
			break;

		case B_32_BIT_640x480:
		case B_32_BIT_800x600:
		case B_32_BIT_1024x768:
		case B_32_BIT_1152x900:
		case B_32_BIT_1280x1024:
		case B_32_BIT_1600x1200:
			return 32;
			break;
		
		default:
			return 8;		// on rend le plus petit, au cas où
	}
}

uint32 ScreenConfig::GetSpace(ScreenResolution res, unsigned int bitsPPixel)
{
	const uint32 *spaces;
	static const uint32 
		sp8[] = { B_8_BIT_640x480, B_8_BIT_800x600, B_8_BIT_1024x768,
				B_8_BIT_1152x900, B_8_BIT_1280x1024, B_8_BIT_1600x1200 },
		sp16[] = { B_16_BIT_640x480, B_16_BIT_800x600, B_16_BIT_1024x768,
				B_16_BIT_1152x900, B_16_BIT_1280x1024, B_16_BIT_1600x1200 },
		sp32[] = { B_32_BIT_640x480, B_32_BIT_800x600, B_32_BIT_1024x768,
				B_32_BIT_1152x900, B_32_BIT_1280x1024, B_32_BIT_1600x1200 };

	switch(bitsPPixel)
	{
		case 8:
			spaces = sp8;
			break;

		case 15:
		case 16:
			spaces = sp16;
			break;

		case 32:
			spaces = sp32;
			break;
		
		default:
			return B_8_BIT_640x480;
	}
	
	return spaces[res];
}

/*uint32 ScreenConfig::GetPriority(unsigned int priorityNum)
{
	switch(priorityNum)
	{
		case 0:
			return 3;
		break;
		
		case 1:
			return 8;
		break;
		
		case 3:
			return 12;
		break;
		
		case 4:
			return 22;
		break;
		
		case 2:
		default:
			return 10;
		break;
	}
	return 0;
}
*/