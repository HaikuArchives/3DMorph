/* écran de veille 3dMorph */
//#define _SNAP
/* configuration des objets 3d utilisés par l'écran de veille */

#include "MorphObjects.h"
#include "morphR45.h"
#include <InterfaceKit.h>
#include "resource3D.h"
#include "list3D.h"
#include "file3D.h"
#include <TranslatorRoster.h>
#include <BitmapStream.h>


// la liste des objets 3D
ObjectsList::ObjectsList(BRect frame,MorphSaver *s)
	: BListView(frame,NULL)
{
	saver = s;
}

ObjectsList::~ObjectsList()
{
}

void ObjectsList::SelectionChanged()
{
	saver->BlockOnObject((unsigned int)CurrentSelection());
	Window()->PostMessage('UPDB',Parent());
}

// les éléments de la liste
ObjectsItem::ObjectsItem(const char *label,bool a)
{
	name = label;
	active = a;
}

ObjectsItem::~ObjectsItem()
{
}

void ObjectsItem::DrawItem(BView *owner,BRect frame,bool complete)
{
	rgb_color col;
	rgb_color dGray = {152,152,152,0};
	rgb_color black = {0,0,0,0};
	
// selectin de la couleur de fond
	if (IsSelected())
		col = dGray;
	else
		col = owner->ViewColor();

// dessin du fond, si nécessaire
	if (IsSelected() || complete)
	{
		owner->SetHighColor(col);
		owner->FillRect(frame);
	}
	owner->SetHighColor(black);
	
// dessin du petit rond si l'objet est actif
	owner->SetLowColor(col);
	frame.right = frame.left+(frame.Height());
	frame.InsetBy(3,3);
	if (active)
		owner->FillEllipse(frame);

// dessin du nom de l'objet
	owner->MovePenTo(frame.right+10,frame.bottom);
	owner->DrawString(name.String());
}
		
// le tab qui gère le choix d'objets 3D
// on a besoin de changer le saver quand il est sélectionné

ObjectsTab::ObjectsTab(MorphSaver *s,float *f,MorphObjects *m)
{
	saver = s;
	saveSpeed = f;
	mo = m;
}

ObjectsTab::~ObjectsTab()
{
}

void ObjectsTab::Select(BView *owner)
{
	BTab::Select(owner);

	saver->BlockOnObject(0);
	owner->Window()->Lock();
	((MorphObjects*)View())->objectsList->Select(0);
	owner->Window()->Unlock();
	
#ifndef _SNAP
	saver->ToggleSpeed0(true);
#endif
}

void ObjectsTab::Deselect()
{
	BTab::Deselect();
	saver->Unblock();

#ifndef _SNAP
	saver->ToggleSpeed0(false);
#endif	
	
	mo->PutConfig();
	mo->myPanel.Hide();
}

MorphObjects::MorphObjects(BRect r,Resource3D *r3D,MorphSaver *s)
	: BView(r,NULL,B_FOLLOW_ALL,B_WILL_DRAW)
{
	res3D = r3D;
	saver = s;

	SetViewColor(216,216,216,0);
		
	// on pose la liste
	r = Bounds();
	r.top += 8;
	r.bottom -= 18;
	r.left += 8;
	r.right = r.left+100;
	
	objectsList = new ObjectsList(r,saver);
	AddChild(new BScrollView(NULL,objectsList,B_FOLLOW_ALL,0,false,true));
	
	unsigned int n = res3D->NumRealObjects();
	bool *config = new bool[n];
	res3D->SaveConfig(config);
	BString name;
	
	for (unsigned int i=0; i<n; i++)
	{
		res3D->GetName(i,name);
		ObjectsItem *obj = new ObjectsItem(name.String());
		obj->active = config[i];
		objectsList->AddItem(obj);
	}

	delete config;
	config = NULL;
	// on met les boutons
	r = Bounds();
	r.top += 8;
	r.bottom = r.top + 24;
	r.left += 132;
	r.right -= 8;
	
	enButton = new BButton(r,NULL,"Disable",new BMessage('ENDS'));
	AddChild(enButton);
	
	r.OffsetBy(0,32);
	delButton = new BButton(r,NULL,"Delete",new BMessage('DELO'));
	AddChild(delButton);
	
	r.OffsetBy(0,32);
	ldButton = new BButton(r,NULL,"Add...",new BMessage('ADDO'));
	AddChild(ldButton);
	
#ifdef _SNAP
	r.OffsetBy(0,64);
	bmButton = new BButton(r,NULL,"Snapshot",new BMessage('SNAP'));
	AddChild(bmButton);
#endif
	
	numActiveObjects = res3D->NumObjects();
}

MorphObjects::~MorphObjects()
{
	PutConfig();
}

void MorphObjects::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'ENDS':
		{
			ObjectsItem *it = objectsList->ItemSelected();

			// il doit y avoir au moins 3 objets actifs			
			if (it->active && numActiveObjects <= 3)
			{
				BAlert *myAlert = new BAlert("3D Morph","You must have at least 3 active objects","OK");
				myAlert->Go(NULL);
				break;
			}
			
			// changement de l'état de l'objet
			it->active = !(it->active);
			objectsList->Invalidate();

			// mide à jour du compte d'objets actifs
			if (it->active)
				numActiveObjects++;
			else
				numActiveObjects--;				
			
			// sélection de l'objet suivant dans la liste, si on peut
			int n = objectsList->CurrentSelection();
			if (n<objectsList->CountItems()-1)
				objectsList->Select(n+1);
			
			// mise à jour du bouton 'enable'-'disable'
			UpdateEnButton();
		}
		break;
		
		case 'UPDB':
			UpdateEnButton();
		break;
		
		case 'DELO':
		{
			// il doit y avoir au moins 3 objets dans la liste
			if (objectsList->CountItems() <= 3)
			{
				BAlert *myAlert = new BAlert("3DMorph","You must have at least 3 objects","OK");
				myAlert->Go(NULL);
				break;
			}
			
			// mise à jour du décompte d'objets actifs
			if (objectsList->ItemSelected()->active)
			{
				numActiveObjects--;
				// s'il y a moins de 3 objets actifs, on en réactive un au pif
				
				while (numActiveObjects<3)
					for (int i=0; i<objectsList->CountItems(); i++)
						if (!((ObjectsItem*)objectsList->ItemAt(i))->active)
						{
							((ObjectsItem*)objectsList->ItemAt(i))->active = true;
							objectsList->Invalidate();
							numActiveObjects++;
							break;
						}
			}
				
			int n = objectsList->CurrentSelection(),nn = n;

			if (nn>=objectsList->CountItems()-1)
				nn = objectsList->CountItems()-2;
			else
				nn++;
			
			objectsList->Select(nn);
			
			Window()->Lock();
			objectsList->RemoveItem(n);
			Window()->Unlock();

			res3D->RemoveObject(n);
		
			UpdateEnButton();
		}
		break;	
		
		case 'ADDO':
			myPanel.Show();
		break;
		
#ifdef _SNAP
		case 'SNAP':
			Snap();
		break;
#endif
		
		case B_REFS_RECEIVED:
		{
			entry_ref objectRef;
			unsigned int objectNum = 0;
			
			while(message->FindRef("refs",objectNum++,&objectRef) == B_OK)
				if (LoadObject(objectRef))
				{
					objectsList->Select(objectsList->CountItems()-1);
					numActiveObjects++;
				}
		}
		
		default:
			BView::MessageReceived(message);
	}
}

void MorphObjects::AttachedToWindow()
{
	enButton->SetTarget(this);
	delButton->SetTarget(this);
	ldButton->SetTarget(this);
	
#ifdef _SNAP
	bmButton->SetTarget(this);
#endif

	myPanel.SetTarget(BMessenger(this));
}

void MorphObjects::UpdateEnButton()
{
	Window()->Lock();
	if (objectsList->ItemSelected()->active)
		enButton->SetLabel("Disable");
	else
		enButton->SetLabel("Enable");
	Window()->Unlock();
}

bool MorphObjects::LoadObject(entry_ref ref)
{
	BEntry entry(&ref);
	File3D object(entry);
	
	if (object.InitCheck() != B_OK)
	{
		BAlert *myAlert = new BAlert("3D Morph","Cannot open file!","Oups!");
		myAlert->Go(NULL);
		return false;
	}
	
	int n = object.NumPoints();
	List3D *points = new List3D(n);
	rgb_color *colors = new rgb_color[n];
	
	if (object.LoadInto(*points,colors) != B_OK)
	{
		BAlert *myAlert = new BAlert("3D Morph","Invalid file!","Oups!");
		myAlert->Go(NULL);
		delete points;
		delete colors;
		return false;
	}
	
	if (!res3D->AddObject(ref.name,*points,colors))
	{
		BString txt = "There is already an object called ";
		txt << ref.name << " in the list. Please rename the file";
		BAlert *myAlert = new BAlert("3D Morph",txt.String(),"OK");
		myAlert->Go();
		delete points;
		delete colors;
		return false;
	}
	
	ObjectsItem *obj = new ObjectsItem(ref.name);
	obj->active = true;
	objectsList->AddItem(obj);
	
	return true;
}

void MorphObjects::PutConfig()
{
	unsigned int n = res3D->NumRealObjects();
	bool *config = new bool[n];
	for (unsigned int i=0; i<n; i++)
		config[i] = ((ObjectsItem*)(objectsList->ItemAt(i)))->active;
	
	res3D->LoadConfig(config);

	res3D->Save();

	delete config;
	config = NULL;
}

#ifdef _SNAP
void MorphObjects::Snap()
{
	BBitmap *snap = saver->DoBitmap(BRect(0,0,320,240));
	
	// cherche le translator qui sauve en targa
	translator_id *translators;
	int32 num_translators;
	uint32 translationType = 0;
	
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	
	roster->GetAllTranslators(&translators,&num_translators);
	
	for (int32 i=0; i<num_translators;i++)
	{
		const translation_format *fmts;
		int32 num_fmts;
		
		roster->GetOutputFormats(translators[i],&fmts,&num_fmts);
		
		for (int32 j=0; j<num_fmts; j++)
			if (!strcasecmp(fmts[j].MIME,"image/x-targa"))
				translationType = fmts[j].type;
	}
	
	BBitmapStream stream(snap);
	BFile file("/boot/home/snap.tga",B_CREATE_FILE|B_WRITE_ONLY);
	roster->Translate(&stream,NULL,NULL,&file,translationType);

	delete snap;
}
#endif //_SNAP