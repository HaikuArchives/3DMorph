// Rotozoom
// fenêtre principale

#include <WindowScreen.h>
#include <Screen.h>
#include <Locker.h>

class MorphSaver;
class MyScreen : public BWindowScreen
{
public:
	MyScreen(MorphSaver*,status_t *error,uint32 space);
	~MyScreen();
	
	void ScreenConnected(bool connected);
	
	void MessageReceived(BMessage *message);

#ifdef _USE_HOOK
	graphics_card_hook GetRectHook();
#endif
	
public:
	BLocker videoAccess;
	sem_id sync;
	
private:
	MorphSaver *saver;
	BScreen *screen;
	bool isConnected;
	uint32 space;
	unsigned int x,y;
};