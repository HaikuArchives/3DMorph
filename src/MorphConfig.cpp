/* écran de veille 3dMorph */

/* commencé le 15 décembre 1998 par Sylvain Tertois */
/* classe pour une nouvelle configuration un peu meilleure
   commencée le 15 juillet 1999 */

#include "MorphConfig.h"

MorphConfig::MorphConfig(BView *inView,float *params)
{
	BRect frame = inView->Bounds();
	const float myLeft = frame.left;
	const float myRight = frame.right;
	const float myMiddle = (myLeft+myRight)/2;

	frame.top += 5;
	frame.bottom = frame.top + 40.0;
	frame.right = myMiddle-5;
	config1 = new AutoSlider(frame,B_EMPTY_STRING,"Morphing speed",0,100,params);
	inView->AddChild(config1);
	config1->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config1->SetHashMarkCount(11);
	config1->SetKeyIncrementValue(10);
	
	frame.left = myMiddle+5;
	frame.right = myRight;
	config5 = new AutoSlider(frame,B_EMPTY_STRING,"Morphing pause",0,100,params+4);
	inView->AddChild(config5);
	config5->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config5->SetHashMarkCount(11);
	config5->SetKeyIncrementValue(10);
	
	frame.top = frame.bottom;
	frame.bottom = frame.top + 40.0;
	frame.left = myLeft;
	frame.right = myMiddle - 5;
	config2 = new AutoSlider(frame,B_EMPTY_STRING,"Movement speed",0,100,params+2);
	inView->AddChild(config2);
	config2->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config2->SetHashMarkCount(11);
	config2->SetKeyIncrementValue(10);
	
	frame.left = myMiddle + 5;
	frame.right = myRight;
	config3 = new AutoSlider(frame,B_EMPTY_STRING,"Rotation speed",0,100,params+1);
	inView->AddChild(config3);
	config3->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config3->SetHashMarkCount(11);
	config3->SetKeyIncrementValue(10);

	frame.top = frame.bottom;
	frame.bottom = frame.top + 40;
	frame.left = myLeft;
	frame.right = myMiddle-5;
	BSlider *l_Slider = new BSlider(frame,B_EMPTY_STRING,"Later...",NULL,0,100,
		B_TRIANGLE_THUMB);
	inView->AddChild(l_Slider);
	l_Slider->SetEnabled(false);
	l_Slider->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	l_Slider->SetHashMarkCount(11);
	l_Slider->SetKeyIncrementValue(10);
	
	frame.left = myMiddle+5;
	frame.right = myRight;
	config8 = new AutoSlider(frame,B_EMPTY_STRING,"Object Size",0,100,params+7);
	inView->AddChild(config8);
	config8->SetHashMarks(B_HASH_MARKS_BOTTOM);
	config8->SetHashMarkCount(11);
	config8->SetKeyIncrementValue(10);
	
	frame.top = frame.bottom;
	frame.bottom = frame.top + 40.0;
	frame.left = myLeft;
	frame.right = myMiddle - 5;
	config6 = new AutoSlider(frame,B_EMPTY_STRING,"Perspective",0,100,params+5);
	inView->AddChild(config6);
	config6->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config6->SetHashMarkCount(11);
	config6->SetKeyIncrementValue(10);
	
	frame.left = myMiddle + 5;
	frame.right = myRight;
	config7 = new AutoSlider(frame,B_EMPTY_STRING,"Depth",0,100,params+6);
	inView->AddChild(config7);
	config7->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config7->SetHashMarkCount(11);
	config7->SetKeyIncrementValue(10);
	
	frame.top = frame.bottom;
	frame.bottom = frame.top + 48.0;
	frame.left = myLeft;
	frame.right = myRight;
	config4 = new AutoSlider(frame,B_EMPTY_STRING,"Morphing Type",0,100,params+3);
	inView->AddChild(config4);
	config4->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	config4->SetHashMarkCount(11);
	config4->SetLimitLabels("Uniform", "Progressive");
	config4->SetKeyIncrementValue(10);
}

MorphConfig::~MorphConfig()
{
}

AutoSlider::AutoSlider(BRect frame,const char *name,const char *label,
		int32 min, int32 max, float *val,thumb_style thumbType,
		uint32 resizingMode)
  : BSlider(frame,name,label,NULL,min,max,thumbType,resizingMode)
{
	SetMessage(new BMessage('SliC'));
	value = val;
	SetPosition(*value);
}

void AutoSlider::AttachedToWindow()
{
	SetTarget(this);
	
	BSlider::AttachedToWindow();
}

void AutoSlider::MessageReceived(BMessage *message)
{
	if (message->what == 'SliC')
		*value = Position();
	else
		BSlider::MessageReceived(message);
}