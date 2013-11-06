/* écran de veille 3dMorph */

/* configuration des objets 3d utilisés par l'écran de veille */

#ifndef MORPHOBJECTS_H
#define MORPHOBJECTS_H

class BView;
class Resource3D;
class MorphSaver;

#include <View.h>
#include <TabView.h>
#include <ListView.h>
#include <ListItem.h>
#include <String.h>
#include "FilePanel.h"

class ObjectsItem : public BListItem
{
public:
	ObjectsItem(const char *label,bool active = true);
	~ObjectsItem();
	
	virtual void DrawItem(BView *owner,BRect frame,bool complete = false);
	bool active;
	
private:
	BString name;
};

class ObjectsList : public BListView
{
public:
	ObjectsList(BRect frame,MorphSaver*);
	~ObjectsList();
	
	void SelectionChanged();
	inline ObjectsItem *ItemSelected() { return (ObjectsItem*)ItemAt(CurrentSelection()); }

private:
	MorphSaver *saver;
};

class MorphObjects;
class ObjectsTab : public BTab
{
public:
	ObjectsTab(MorphSaver*,float*,MorphObjects*);
	~ObjectsTab();
	
	void Deselect();
	void Select(BView*);

private:
	MorphSaver *saver;
	float *saveSpeed;
	float saveValue;
	MorphObjects *mo;
};
	
class MorphObjects : public BView
{
public:
	MorphObjects(BRect r,Resource3D *res,MorphSaver *saver);
	~MorphObjects();
	
	void MessageReceived(BMessage*);
	void AttachedToWindow();
	void UpdateEnButton();
	
	void PutConfig();
	void Snap();
			
private:

	bool LoadObject(entry_ref);
	
	ObjectsList *objectsList;
	Resource3D *res3D;
	BButton *enButton,*delButton,*ldButton,*bmButton;
	unsigned int numActiveObjects;
	BFilePanel myPanel;
	MorphSaver *saver;

	friend class ObjectsTab;
};

#endif //MORPHOBJECTS_H