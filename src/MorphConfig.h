/* écran de veille 3dMorph */

/* commencé le 15 décembre 1998 par Sylvain Tertois */
/* classe pour une nouvelle configuration un peu meilleure
   commencée le 15 juillet 1999 */

#ifndef _MORPHCONFIG
#define _MORPHCONFIG

#include <InterfaceKit.h>

class AutoSlider : public BSlider	// slider qui modifie tout seul un float
{
public:
	AutoSlider(BRect frame,const char *name,const char *label,
		int32 min, int32 max, float *value,thumb_style thumbType = B_TRIANGLE_THUMB,
		uint32 resizingMode = B_FOLLOW_LEFT|B_FOLLOW_TOP);
		
		void AttachedToWindow();
		void MessageReceived(BMessage*);

private:
	float *value;
};

class MorphConfig
{
public:
	MorphConfig(BView*,float*);	// création de la vue de config
	~MorphConfig();
	
private:
	BView *mainView;
	float *params;
	AutoSlider *config1,*config2,*config3,*config4,*config5,*config6,*config7;
	AutoSlider *config8;
};

#endif //_MORPHCONFIG