// 3D Morph
// Fenetre WindowScreen

#include "MyScreen.h"
#include <Application.h>
#include "morphR45.h"
#include "ScreenConfig.h"
#include <stdlib.h>
#include <Beep.h>

MyScreen::MyScreen(MorphSaver *ms,status_t *error,uint32 sp)
	: BWindowScreen("3D Morph",sp,error)
{
	isConnected = false;
	screen = NULL;
	saver = ms;
	sync = create_sem(0,"WindowScreen sync");
	space = sp;

	Show();
	*error = B_OK;
	
	switch(ScreenConfig::GetResolution(space))
	{
		case S_640x480:
			x = 640;
			y = 480;
			break;
		
		case S_800x600:
			x = 800;
			y = 600;
			break;
			
		case S_1024x768:
			x = 1024;
			y = 768;
			break;
		
		case S_1152x900:
			x = 1152;
			y = 900;
			break;
		
		case S_1280x1024:
			x = 1280;
			y = 1024;
			break;
		
		case S_1600x1200:
			x = 1600;
			y = 1200;
			break;
		
		default:
			x = 640;
			y = 480;
			break;
	}
}

MyScreen::~MyScreen()
{
	delete_sem(sync);
}

void MyScreen::ScreenConnected(bool connected)
{
	videoAccess.Lock();
	isConnected = connected;
	if (connected)
	{
		if (SetFrameBuffer(x,y*2) != B_OK)
		{
			PostMessage(B_QUIT_REQUESTED);
			videoAccess.Unlock();
			return;
		}
		screen = new BScreen(this);
		release_sem(sync);
		
		// remplissage de l'écran avec du noir
		size_t sz = CardInfo()->bytes_per_row * CardInfo()->height;
		memset(CardInfo()->frame_buffer,0x00,sz);
	}
	else
	{
		delete screen;
		screen = NULL;
	}

	videoAccess.Unlock();

	saver->IsConnected(connected,CardInfo()->frame_buffer,space);
}

void MyScreen::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'DRAW':
		{
			uint32 imageNum;
			if (message->FindInt32("image num",(int32*)&imageNum) != B_OK)
				imageNum = 0;

			videoAccess.Lock();
			if (isConnected)
			{
				MoveDisplayArea(0,y*imageNum);
				screen->WaitForRetrace();
			}
			videoAccess.Unlock();
			release_sem(sync);
		}
		break;

		default:
			BWindowScreen::MessageReceived(message);
	}
}

#ifdef _USE_HOOK
graphics_card_hook MyScreen::GetRectHook()
{
	switch (ScreenConfig::GetBitsPPixel(space))
	{
		case 8:
			return CardHookAt(5);
		
		case 15:
		case 16:
			return CardHookAt(13);
		
		case 24:
		case 32:
			return CardHookAt(6);
		
		default:
			return NULL;
	}
}
#endif