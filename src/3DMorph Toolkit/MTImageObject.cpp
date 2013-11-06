/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTImageObject class
 * Object Translator that loads image files, and creates 3D Morph objects
 * with them
 *
**/

#include "MTImageObject.h"
#include "MTPoints.h"
#include "MTVector.h"
#include "Mainwin.h"
#include "MTMatrix.h"
#include <Alert.h>
#include <Mime.h>
#include <Box.h>
#include <Button.h>
#include <String.h>
#include <FilePanel.h>
#include <File.h>
#include <NodeInfo.h>
#include <DataIO.h>
#include <TranslationUtils.h>
#include <Bitmap.h>
#include <RadioButton.h>
#include <TextControl.h>
#include <stdlib.h>
#include <Slider.h>
#include <CheckBox.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <MenuItem.h>

class MTIO_ConfigView : public BBox
{
public:
	MTIO_ConfigView(MTImageObject*);
	~MTIO_ConfigView();
	
	virtual void MessageReceived(BMessage *);
	virtual void AttachedToWindow();
	void UpdateImageViews();	// met à jour les images réduites à côté des boutons
	BBox *CreateBox(const char **choices, uint32 notification, 
		BTextControl *lastModifier);	// crée une BBox avec des options
			// (utilisée pour les BBox X points et Y points)
	void UpdateXYBoxes();	// met à jour le contenu des boites X et Y
	BMenu *CreateMenu(const char **choices, uint32 notification);
			// crée un PopupMenu avec les choix que l'on donne

	MTImageObject *m_Object;
	BButton *m_ButtonIMG,*m_ButtonHeight;
	BView *m_ViewIMG,*m_ViewHeight;
	BBox *m_xpBox, *m_ypBox;
	BTextControl *m_xpText,*m_ypText;
	BSlider *m_HeightSlider;
	BCheckBox *m_PropCheckBox,*m_IgnoreCheckBox;
	BMenuField *m_OrderMenu,*m_PlaneMenu;
};

MTImageObject::MTImageObject()
{
	m_ImageBitmap = NULL;
	m_HeightFieldBitmap = NULL;
	m_ConfigView = NULL;
	
	m_xParams = X_32;
	m_yParams = Y_PROP;

	m_HeightAmplitude = 1;
	
	m_PropObject = true;
	m_IgnoreBlackPoints = false;
	m_Order = O_HORIZ;
	m_Plane = P_XY;
}

MTImageObject::~MTImageObject()
{
	delete m_ImageBitmap;
	delete m_HeightFieldBitmap;
}

bool MTImageObject::CanHandleType(const char *type)
{
	BMimeType l_MimeType(type);
	BMimeType l_SuperType;
	return ((l_MimeType.GetSupertype(&l_SuperType) == B_OK) &&
			(l_SuperType == "image"));
}

void MTImageObject::Load(const char *mimeType, const void *data, size_t size)
{
	// création d'un objet BMemoryIO qui contiendra l'image
	// (indispensable avec les translators)
	BMemoryIO l_image(data,size);
	BBitmap *l_newBitmap = BTranslationUtils::GetBitmap(&l_image);
	if (l_newBitmap == NULL)
		(new BAlert("3DMorph image loader","Unable to load image","Cancel",
			NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
	else
	{
		delete m_ImageBitmap;
		m_ImageBitmap = l_newBitmap;
		
		// mise à jour de la vue qui présente l'image réduite
		if (m_ConfigView != NULL)
		{
			m_ConfigView->UpdateImageViews();
			m_ConfigView->UpdateXYBoxes();
		}
	}
}

#define SIZE 32

MTPoints *MTImageObject::Update(float)
{
	if (m_ImageBitmap != NULL)
	{
		unsigned int l_SizeX,l_SizeY;
		CalcSize(&l_SizeX,&l_SizeY);
		
		// création du bitmap qui va contenir l'image, tout comme il faut
		BBitmap l_image(BRect(0,0,l_SizeX-1,l_SizeY-1),B_RGB32,true);
		BView l_drawView(BRect(0,0,l_SizeX-1,l_SizeY-1),NULL,B_FOLLOW_NONE,0);
		l_image.AddChild(&l_drawView);
		
		// on va dessiner le bitmap chargé dans celui-ci, qui a la bonne
		// taille et le bon colorspace. On a besoin du looper associé
		// à la view pour le bloquer
		BLooper *l_theLooper = l_drawView.Looper();
		l_theLooper->Lock();
		l_drawView.DrawBitmap(m_ImageBitmap,BRect(0,0,l_SizeX-1,l_SizeY-1));
		l_drawView.RemoveSelf();
		l_theLooper->Unlock();

		// allocation des espaces pour stoquer les couleurs des points
		unsigned int l_nPoints = l_SizeX*l_SizeY;
		float *l_Reds = new float[l_nPoints];
		float *l_Greens = new float[l_nPoints];
		float *l_Blues = new float[l_nPoints];
		
		// on prend les couleurs
		uint32 *l_bits = (uint32*)l_image.Bits();
		for (unsigned int i=0; i<l_nPoints; i++)
		{
			l_Reds[i] = (((*l_bits) >> 16) & 0xff) / 255.0;
			l_Greens[i] = (((*l_bits) >> 8) & 0xff) / 255.0;
			l_Blues[i] = ((*l_bits) & 0xff) / 255.0;
			
			l_bits++;
		}
				
		// on les place dans des vecteurs
		MTVector l_VR(l_nPoints,l_Reds);
		delete l_Reds;
		MTVector l_VG(l_nPoints,l_Greens);
		delete l_Greens;
		MTVector l_VB(l_nPoints,l_Blues);
		delete l_Blues;
		
		// on détermine la taille que va devoir avoir l'objet
		float l_ObXSize = 1, l_ObYSize = 1;
		if (m_PropObject)
		{
			float w = m_ImageBitmap->Bounds().Width();
			float h = m_ImageBitmap->Bounds().Height();
			
			if (w>h)
				l_ObYSize = h/w;
			else
				l_ObXSize = w/h;
		}
		
		// on crée les vecteurs de coordonnées
		MTVector l_VX = MTVector::GradientInc(-l_ObXSize,l_ObXSize,l_SizeX);
		MTVector l_VY = MTVector::GradientInc(l_ObYSize,-l_ObYSize,l_SizeY);
		l_VX.Repeat(1,l_SizeY);
		l_VY.Repeat(l_SizeX,1);
		MTVector l_VZ = MTVector::Constant(0,l_nPoints);
		
		// s'il y a un height field, il est temps de s'en occuper
		if (m_HeightFieldBitmap != NULL)
		{
			/// on place le height field dans un bitmap en niveaux de gris
			/// de la bonne taille
			BBitmap l_image(BRect(0,0,l_SizeX-1,l_SizeY-1),B_RGB32,true);
			BView l_drawView(BRect(0,0,l_SizeX-1,l_SizeY-1),NULL,
				B_FOLLOW_NONE,0);
			l_image.AddChild(&l_drawView);
		
			/// on va dessiner le bitmap chargé dans celui-ci, qui a la bonne
			/// taille et le bon colorspace. On a besoin du looper associé
			/// à la view pour le bloquer
			BLooper *l_theLooper = l_drawView.Looper();
			l_theLooper->Lock();
			l_drawView.DrawBitmap(m_HeightFieldBitmap,
				BRect(0,0,l_SizeX-1,l_SizeY-1));
			l_drawView.RemoveSelf();
			l_theLooper->Unlock();
						
			/// mise à jour des valeurs dans le vecteur
			uint32 *l_bits = (uint32*)l_image.Bits();
			for (unsigned int i=0; i<l_nPoints; i++)
			{
				l_VZ[i] = ((((*l_bits >> 16) & 0xff)*0.2) + 
					(((*l_bits >> 8) & 0xff)*0.7) +	
					((*l_bits & 0xff)*0.1))/127.5-1;
			
				l_bits++;
			}
			
			l_VZ *= m_HeightAmplitude;
		}
		
		// s'il y a besoin, on réorganise les points
		if ((m_Order != O_HORIZ) || m_IgnoreBlackPoints)
		{
			MTVector l_reorg;
			switch(m_Order)
			{
				case O_HORIZ:
					l_reorg = MTVector::GradientExc(0,l_nPoints,l_nPoints);
					break;
				
				case O_VERT:
				{
					l_reorg = MTVector::GradientExc(0,l_SizeY,l_SizeY);
					l_reorg *= l_SizeX;
					l_reorg.Repeat(1,l_SizeX);
					MTVector tmp = MTVector::GradientExc(0,l_SizeX,l_SizeX);
					tmp.Repeat(l_SizeY,1);
					l_reorg += tmp;
					break;
				}
				
				case O_ZIGZAG:
				{
					if (l_SizeY > 1)
					{
						l_reorg = MTVector::GradientExc(0,l_SizeX,l_SizeX);
						l_reorg &= MTVector::GradientInc(l_SizeX-1,0,l_SizeX);
						l_reorg.Repeat(1,l_SizeY/2);
					}
					if ((l_SizeY % 2) == 1)
						l_reorg &= MTVector::GradientExc(0,l_SizeX,l_SizeX);
					MTVector tmp = MTVector::GradientExc(0,l_SizeY,l_SizeY);
					tmp *= l_SizeX;
					tmp.Repeat(l_SizeX,1);
					l_reorg += tmp;
					break;
				}
				
				case O_RAND:
					l_reorg = MTVector::GradientExc(0,l_nPoints,l_nPoints);
					
					// on mélange le tout
					for (unsigned int i=0; i<l_nPoints*4; i++)
					{
						unsigned int n1 = (rand() & 0x7fff)*l_nPoints/0x7fff;
						unsigned int n2 = (rand() & 0x7fff)*l_nPoints/0x7fff;
						float f = l_reorg[n1];
						l_reorg[n1]=l_reorg[n2];
						l_reorg[n2]=f;
					}
					break;				
			}
			
			// on regarde s'il faut supprimer les points noirs
			if (m_IgnoreBlackPoints)
			{
				float *l_realReorg=new float[l_nPoints];
				unsigned int l_nNonBlackPoints = 0;
				for (unsigned int i=0; i<l_nPoints; i++)
				{
					unsigned int j = (unsigned int)l_reorg[i];
					if ((l_VR[j] != 0) || (l_VG[j] != 0) || (l_VB[j]))
						l_realReorg[l_nNonBlackPoints++]= (float)j;
				}
				
				l_reorg.SetTo(l_nNonBlackPoints,l_realReorg);
				delete l_realReorg;
			}
			
			// et on réorganise les points comme il le faut
			l_VX = l_reorg^l_VX;
			l_VY = l_reorg^l_VY;
			l_VZ = l_reorg^l_VZ;
			l_VR = l_reorg^l_VR;
			l_VG = l_reorg^l_VG;
			l_VB = l_reorg^l_VB;
						
		}
		
		MTPoints *l_Points = new MTPoints(l_VX,l_VY,l_VZ,l_VR,l_VG,l_VB); 
		// S'il y a besoin, on fait une rotation pour mettre l'image dans le
		// bon plan
		if (m_Plane != P_XY)
		{
			MTMatrix l_Trans;
			switch(m_Plane)
			{
				case P_XY:
					break;
					
				case P_XZ:
					l_Trans = MTMatrix::RotateX(3.141592/2);
					break;
				
				case P_YZ:
					l_Trans = MTMatrix::RotateY(3.141592/2);
					break;
			}
			
			*l_Points *= l_Trans;
		}
				
		return l_Points;
	}
	else
		return new MTPoints;
}

BView *MTImageObject::CreateView()
{
	return (m_ConfigView = new MTIO_ConfigView(this));
}

void MTImageObject::CalcSize(unsigned int *x, unsigned int *y)
{
	switch(m_xParams)
	{
		case X_32:
			*x = 32;
			break;
		
		case X_IMAGE:
			*x = (unsigned int)m_ImageBitmap->Bounds().Width();
			break;
		
		case X_OTHER:
			if (m_ConfigView != NULL)
			{
				int l_i = atoi(m_ConfigView->m_xpText->Text());
				if (l_i<=0)
					*x = 1;
				else
					*x = (unsigned int)l_i;
			}
			else
				*x = 1;
			break;
	}
	
	switch(m_yParams)
	{
		case Y_PROP:
			*y = (unsigned int)((*x * m_ImageBitmap->Bounds().Height())/
				m_ImageBitmap->Bounds().Width());
			if (*y<1)
				*y=1;
			break;
		
		case Y_X:
			*y = *x;
			break;
		
		case Y_IMAGE:
			*y = (unsigned int)(m_ImageBitmap->Bounds().Height());
			break;
		
		case Y_OTHER:
			if (m_ConfigView != NULL)
			{
				int l_i = atoi(m_ConfigView->m_ypText->Text());
				if (l_i<=0)
					*y = 1;
				else
					*y = (unsigned int)l_i;
			}
			else
				*y = 1;
			break;
	}
}
	
static const float c_margin = 10;
static const char *c_xpOptions[] = {"32","same as image","other:",NULL};
static const char *c_ypOptions[] = {"proportionnal","same as X","same as image","other:",NULL};
static const char *c_orderOptions[] = {"Horizontal","Vertical","Zig-zag","Random",NULL};
static const char *c_planeOptions[] = {"XY", "XZ", "YZ", NULL };

MTIO_ConfigView::MTIO_ConfigView(MTImageObject *object)
	: BBox(BRect(0,0,350,60),NULL,B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP),
	  m_Object(object)
{
	// Boite générale
	SetLabel("Image Object");
	
	// boutons
	/// ces flottants vont servir à placer les vues
	float l_left = c_margin,l_right;
	float l_top = c_margin+6;
	float l_top2,l_ButtonSize,l_bottom;
	
	/// bouton 'image'
	BRect l_rect(l_left,l_top,l_left+100,l_top+20);
	m_ButtonIMG = new BButton(l_rect,NULL,"New Image",
		new BMessage('IoNI'));
	AddChild(m_ButtonIMG);
	
	l_top2 = m_ButtonIMG->Frame().bottom+c_margin;
	l_ButtonSize = m_ButtonIMG->Frame().Height();

	/// bouton 'height field'	
	l_rect.Set(l_left,l_top2,l_left+100,l_top2+l_ButtonSize);
	m_ButtonHeight = new BButton(l_rect,NULL,"New Height Image",
		new BMessage('IoNH'));
	//// mise des deux boutons à la même taille, celle du bouton du bas,
	//// qui sera sans doute le plus gros
	m_ButtonHeight->ResizeToPreferred();
	m_ButtonIMG->ResizeTo(m_ButtonHeight->Frame().Width(),l_ButtonSize);
	AddChild(m_ButtonHeight);
	l_bottom = m_ButtonHeight->Frame().bottom;
	
	// vues réduites des images
	l_left = m_ButtonHeight->Frame().right+c_margin;
	l_rect.Set(l_left,l_top,l_left+l_ButtonSize,l_top+l_ButtonSize);
	m_ViewIMG = new BView(l_rect,NULL,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW);
	AddChild(m_ViewIMG);
	l_rect.OffsetTo(l_left,l_top2);
	m_ViewHeight = new BView(l_rect,NULL,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW);
	AddChild(m_ViewHeight);
	
	// slider pour l'importance du height field
	l_rect.left = m_ButtonHeight->Frame().left;
	l_rect.top = l_bottom + c_margin;
	l_rect.bottom = l_rect.top+10;
	m_HeightSlider = new BSlider(l_rect,NULL,"Height amplitude",
		NULL,0,1000);
	m_HeightSlider->SetValue(1000);
	m_HeightSlider->SetModificationMessage(new BMessage('IoHA'));
	m_HeightSlider->ResizeToPreferred();
	m_HeightSlider->SetEnabled(false);
	AddChild(m_HeightSlider);
	l_bottom = m_HeightSlider->Frame().bottom;
	
	
	// paramètres pour le nombre de points
	/// X
	l_left += l_ButtonSize+c_margin;
	m_xpText = new BTextControl(BRect(0,0,40,10),NULL,NULL,"ABCD",NULL);
	m_xpText->ResizeToPreferred();
	m_xpBox = CreateBox(c_xpOptions,'IoXP',m_xpText);
	m_xpBox->MoveTo(l_left,l_top);
	m_xpBox->SetLabel("X Points");
	AddChild(m_xpBox);
	if (m_xpBox->Frame().bottom>l_bottom)
		l_bottom = m_xpBox->Frame().bottom;
		
	/// Y
	l_left = m_xpBox->Frame().right+c_margin;
	m_ypText = new BTextControl(BRect(0,0,40,10),NULL,NULL,"ABCD",NULL);
	m_ypText->ResizeToPreferred();
	m_ypBox = CreateBox(c_ypOptions,'IoYP',m_ypText);
	m_ypBox->MoveTo(l_left,l_top);
	m_ypBox->SetLabel("Y Points");
	AddChild(m_ypBox);
	if (m_ypBox->Frame().bottom>l_bottom)
		l_bottom = m_ypBox->Frame().bottom;
	
	UpdateXYBoxes();
	
	// options supplémentaires
	/// bouton 'proportionnal object'
	l_left = m_ypBox->Frame().right+c_margin;
	l_rect.Set(l_left,l_top,l_left+120,l_top+10);
	m_PropCheckBox = new BCheckBox(l_rect,NULL,"Proportionnal object",
		new BMessage('IoPO'));
	m_PropCheckBox->ResizeToPreferred();
	m_PropCheckBox->SetValue(1);
	AddChild(m_PropCheckBox);
	l_rect.top = m_PropCheckBox->Frame().bottom+c_margin;
	l_right = m_PropCheckBox->Frame().right;
	
	/// bouton 'ignore black points'
	l_rect.bottom = l_rect.top+10;
	m_IgnoreCheckBox = new BCheckBox(l_rect,NULL,"Ignore black points",
		new BMessage('IoIB'));
	m_IgnoreCheckBox->ResizeToPreferred();
	AddChild(m_IgnoreCheckBox);
	l_rect.top = m_IgnoreCheckBox->Frame().bottom+c_margin;
	if (m_IgnoreCheckBox->Frame().right > l_right)
		l_right = m_IgnoreCheckBox->Frame().right;
	
	/// création d'un menu pour l'ordre des points
	l_rect.bottom = l_rect.top+10;
	BMenu *l_menu = CreateMenu(c_orderOptions,'IoOo');
	m_OrderMenu = new BMenuField(l_rect,NULL,"Order:",l_menu);
	m_OrderMenu->ResizeToPreferred();
	m_OrderMenu->SetDivider(45);
	AddChild(m_OrderMenu);
	l_rect.top = m_OrderMenu->Frame().bottom+c_margin;
	
	/// création d'un menu pour le plan de l'image
	l_rect.bottom = l_rect.top+10;
	l_menu = CreateMenu(c_planeOptions,'IoPo');
	m_PlaneMenu = new BMenuField(l_rect,NULL,"Plane:",l_menu);
	m_PlaneMenu->ResizeToPreferred();
	m_PlaneMenu->SetDivider(45);
	AddChild(m_PlaneMenu);
	l_rect.top = m_PlaneMenu->Frame().bottom+c_margin;
	
	if (m_PlaneMenu->Frame().right > l_right)
		l_right = m_PlaneMenu->Frame().right;
	if (m_PlaneMenu->Frame().bottom > l_bottom)
		l_bottom = m_PlaneMenu->Frame().bottom;
	
	// redimensionnement de la vue principale
	ResizeTo(l_right+c_margin,l_bottom+c_margin);
}

MTIO_ConfigView::~MTIO_ConfigView()
{
}

void MTIO_ConfigView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		// appui sur le bouton 'New Image'
		case 'IoNI':
		{
			BMessage l_Notification('IoNt');
			g_FilePanel->SetMessage(&l_Notification);
			
			BMessenger l_Me(this);
			g_FilePanel->SetTarget(l_Me);
			
			g_FilePanel->Show();
		
			break;
		}

		// appui sur le bouton 'New Height Image'
		case 'IoNH':
		{
			BMessage l_Notification('IoHn');
			g_FilePanel->SetMessage(&l_Notification);
			
			BMessenger l_Me(this);
			g_FilePanel->SetTarget(l_Me);
			
			g_FilePanel->Show();
		
			break;
		}
		
		// changement des paramètres X
		case 'IoXP':
		{
			int32 l_number;
			if (message->FindInt32("number",&l_number) == B_OK)
			{
				m_Object->m_xParams = (MTImageObject::XPoints)l_number;
				UpdateXYBoxes();
			}
			break;
		}
		
		// changement des paramètres Y
		case 'IoYP':
		{
			int32 l_number;
			if (message->FindInt32("number",&l_number) == B_OK)
			{
				m_Object->m_yParams = (MTImageObject::YPoints)l_number;
				UpdateXYBoxes();
			}
			break;
		}
		
		// chargement d'une nouvelle image
		case 'IoNt':
		{
			entry_ref l_EntryRef;
			if (message->FindRef("refs",&l_EntryRef) == B_OK)
			{
				// alors, c'est quoi ce fichier?			
				BFile l_File(&l_EntryRef,B_READ_ONLY);
				if (l_File.InitCheck() != B_OK)
				return;

				// maintenant on est sûr que le fichier existe
				// récupération de son type MIME
				BNodeInfo l_FileInfo(&l_File);
				char l_MimeType[B_MIME_TYPE_LENGTH+1];
				if (l_FileInfo.GetType(l_MimeType) != B_OK)
					return;

				// est-ce bien le bon type de fichier?
				if (m_Object->CanHandleType(l_MimeType))
				{
					// chargement des données
					off_t l_Size;
					if ((l_File.GetSize(&l_Size) != B_OK) || (l_Size <= 0))
						return;
					char *l_Buffer = new char[l_Size];
					if (l_File.Read((void*)l_Buffer,l_Size) != l_Size)
						goto fin;

					// envoi des données au translator
					m_Object->Load(l_MimeType,(void*)l_Buffer,l_Size);
					
					// mise à jour de la petite image
					UpdateImageViews();
					UpdateXYBoxes();
					
fin:				delete l_Buffer;
				}
				else
					(new BAlert("Image loader",
						"File must be an image!","Cancel",NULL,NULL,
						B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			}
			break;
		}	
		
		// chargement d'un nouveau 'Height Field'
		case 'IoHn':
		{
			entry_ref l_EntryRef;
			if (message->FindRef("refs",&l_EntryRef) == B_OK)
			{
				BBitmap *l_newBitmap = BTranslationUtils::GetBitmap(&l_EntryRef);
				if (l_newBitmap != NULL)
				{
					m_Object->m_HeightFieldBitmap = l_newBitmap;
					UpdateImageViews();
					m_HeightSlider->SetEnabled(true);
				}
				else
					(new BAlert("3DM Toolkit","can't load image!","Cancel",
						NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();		
			}
			break;
		}			
		
		// modification de la position du slider 'height amplitude'
		case 'IoHA':
			m_Object->m_HeightAmplitude = m_HeightSlider->Position();
			m_Object->RequestUpdate();
			break;
		
		// changement de la checkbox 'proportionnal object'
		case 'IoPO':
			m_Object->m_PropObject = (m_PropCheckBox->Value() != 0);
			break;
			
		// changement de la checkbox 'ignore black points'
		case 'IoIB':
			m_Object->m_IgnoreBlackPoints = (m_IgnoreCheckBox->Value() != 0);
			break;
		
		// changement de la sélection des menus
		/// Ordre des points
		case 'IoOo':
		{
			int32 l_number;
			if (message->FindInt32("number",&l_number) == B_OK)
				m_Object->m_Order = (MTImageObject::Order)l_number;
			break;
		}
		
		/// Plan de l'image
		case 'IoPo':
		{
			int32 l_number;
			if (message->FindInt32("number",&l_number) == B_OK)
				m_Object->m_Plane = (MTImageObject::Plane)l_number;
			break;
		}
		
		default:
			BBox::MessageReceived(message);
			break;
	}
}

void MTIO_ConfigView::AttachedToWindow()
{
	// on fixe les cibles pour les différents objets
	/// boutons
	m_ButtonIMG->SetTarget(this);
	m_ButtonHeight->SetTarget(this);
	
	/// radios (un poil plus complexe)
	int32 l_numRadios = m_xpBox->CountChildren();
	for (int i=0; i<l_numRadios; i++)
		((BControl*)m_xpBox->ChildAt(i))->SetTarget(this);
	l_numRadios = m_ypBox->CountChildren();
	for (int i=0; i<l_numRadios; i++)
		((BControl*)m_ypBox->ChildAt(i))->SetTarget(this);
	
	// slider
	m_HeightSlider->SetTarget(this);
	
	// les deux checkboxes de droite
	m_PropCheckBox->SetTarget(this);
	m_IgnoreCheckBox->SetTarget(this);

	// et les menus
	m_OrderMenu->Menu()->SetTargetForItems(this);
	m_PlaneMenu->Menu()->SetTargetForItems(this);

	UpdateImageViews();
	
	BBox::AttachedToWindow();
}

void MTIO_ConfigView::UpdateImageViews()
{
	if (m_Object->m_ImageBitmap != NULL)
	{
		m_ViewIMG->SetViewBitmap(m_Object->m_ImageBitmap,
			m_Object->m_ImageBitmap->Bounds(),m_ViewIMG->Bounds());
		m_ViewIMG->Invalidate();
	}

	if (m_Object->m_HeightFieldBitmap != NULL)
	{
		m_ViewHeight->SetViewBitmap(m_Object->m_HeightFieldBitmap,
			m_Object->m_HeightFieldBitmap->Bounds(),m_ViewHeight->Bounds());
		m_ViewHeight->Invalidate();
	}
}

static const float c_marginSmall = 5;
BBox *MTIO_ConfigView::CreateBox(const char **choices, uint32 notification, 
		BTextControl *lastModifier)
{
	BBox *l_Box = new BBox(BRect(0,0,10,10));
	float l_width = 0,l_top = c_marginSmall+6;
	
	// création et mise en place des radiobuttons
	BRadioButton *l_button;
	unsigned int l_number = 0;
	while(*choices != NULL)
	{
		/// création du message de notification
		BMessage *l_message = new BMessage(notification);
		l_message->AddInt32("number",l_number++);
		
		/// création du bouton
		l_button = new BRadioButton(BRect(c_margin,l_top,c_margin+10,l_top+10),
			NULL,*choices++,l_message);
		l_button->ResizeToPreferred();
		
		/// mise à jour des variables qui contrôlent la taille
		if (l_button->Frame().right > l_width)
			l_width = l_button->Frame().right;
		l_top = l_button->Frame().bottom+1;
		
		/// ajout de l'objet
		l_Box->AddChild(l_button);
		
		/// s'il y a besoin, on ajoute un BTextControl à côté
		if ((*choices == NULL) && (lastModifier != NULL))
		{
			//// on fait une copie du message envoyé au radiobutton
			l_message = new BMessage(*l_message);
			lastModifier->SetMessage(l_message);
			lastModifier->MoveTo(l_button->Frame().right+c_marginSmall,
				l_button->Frame().top);
			l_Box->AddChild(lastModifier);
			if (lastModifier->Frame().right > l_width)
				l_width = lastModifier->Frame().right;
		}
	}
	
	// tous les boutons sont ajoutés.
	// mise à jour de la taille de la BBox
	l_Box->ResizeTo(l_width+c_marginSmall,l_top+c_marginSmall);
	
	return l_Box;
}	

BMenu *MTIO_ConfigView::CreateMenu(const char **choices, uint32 notification)
{
	BMenu *l_Menu = new BPopUpMenu((const char*)NULL);
	
	// création et mise en place des radiobuttons
	BMenuItem *l_menuItem;
	unsigned int l_number = 0;
	while(*choices != NULL)
	{
		/// création du message de notification
		BMessage *l_message = new BMessage(notification);
		l_message->AddInt32("number",l_number);
		
		/// création de l'item
		l_menuItem = new BMenuItem(*choices++,l_message);
		
		/// ajout au menu
		l_Menu->AddItem(l_menuItem);
		
		// s'il y a besoin, sélectionne le premier item
		if (l_number++ == 0)
			l_menuItem->SetMarked(true);
	}
	
	return l_Menu;
}	

void MTIO_ConfigView::UpdateXYBoxes()
{
	// tout d'abord on cherche les RadioButtons qui doivent être sélectionnés
	// s'ils ne le sont pas, on le fait
	/// X
	BControl *l_radio = (BControl*)m_xpBox->ChildAt((int32)m_Object->m_xParams);
	if (l_radio->Value() == 0)
		l_radio->SetValue(1);
	
	/// Y
	l_radio = (BControl*)m_ypBox->ChildAt((int32)m_Object->m_yParams);
	if (l_radio->Value() == 0)
		l_radio->SetValue(1);
	
	// ensuite on met à jour le contenu des TextControl
	/// recherche de la taille de l'objet
	unsigned int l_x,l_y;
	m_Object->CalcSize(&l_x,&l_y);
	
	/// X
	BString text;
	text << l_x;
	m_xpText->SetText(text.String());
	
	/// Y
	text.SetTo("");
	text << l_y;
	m_ypText->SetText(text.String());
	
	// et enfin on désactive les TextControl si l'utilisateur n'a pas la
	// possibilité de les modifier
	m_xpText->SetEnabled(m_Object->m_xParams == MTImageObject::X_OTHER);
	m_ypText->SetEnabled(m_Object->m_yParams == MTImageObject::Y_OTHER);
}