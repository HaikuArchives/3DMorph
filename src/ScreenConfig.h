// Configuration du dessin

class BCheckBox;
class MorphSaver;

typedef enum { S_640x480 = 0, S_800x600 = 1, S_1024x768 = 2, S_1152x900 = 3,
			   S_1280x1024 = 4, S_1600x1200 = 5} ScreenResolution;

#include <View.h>

class ScreenConfig : public BView
{
public:
	ScreenConfig(BRect r,MorphSaver *saver);
	~ScreenConfig();
	
	void MessageReceived(BMessage*);
	
	void SaveState();
	
	static ScreenResolution GetResolution(uint32 space);
	static unsigned int GetBitsPPixel(uint32 space);
	static uint32 GetSpace(ScreenResolution res, unsigned int bitsPPixel);
//	static uint32 GetPriority(unsigned int priorityNum);
	
	void AttachedToWindow();

private:
	void DoUpdate();

	BCheckBox *cbDoubleBuf;
	BCheckBox *cbWait;
	BCheckBox *cbSoft;
	BCheckBox *cbNice;
	BMenuField *mfResolution, *mfDepth/*, mfPriority*/;
	ScreenResolution selRes;
	uint32 selBits;
//	uint32 selPriority;
	MorphSaver *saver;
	bool attachedDone;
};
