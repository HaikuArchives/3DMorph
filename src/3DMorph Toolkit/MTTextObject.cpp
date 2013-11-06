/*
 * 3D Morph Toolkit
 *
 *  by Sylvain Tertois
 *
 * MTTextObject class
 * Object Translator that loads text files, with the coordinates and colors
 *
**/

#include "MTTextObject.h"
#include "MTPoints.h"
#include "Mainwin.h"
#include <Alert.h>
#include <Mime.h>
#include <Box.h>
#include <Button.h>
#include <String.h>
#include <stdio.h>
#include <FilePanel.h>
#include <File.h>
#include <NodeInfo.h>

typedef struct MTTO_BufferInfo {
	const char *start;
	const char *pointer;
	size_t size;
	} MTTO_BufferInfo;
	
class MTTO_ConfigView : public BBox
{
public:
	MTTO_ConfigView(MTTextObject*);
	~MTTO_ConfigView();
	
	virtual void MessageReceived(BMessage *);
	virtual void AttachedToWindow();

private:
	MTTextObject *m_Object;
	BButton *m_Button;	
};
 
MTTextObject::MTTextObject()
{
	m_Points = NULL;
}

MTTextObject::~MTTextObject()
{
	delete m_Points;
}

bool MTTextObject::CanHandleType(const char *type)
{
	BMimeType l_MimeType(type);
	return l_MimeType == "text/plain";
}

void MTTextObject::Load(const char *mimeType, const void *data, size_t size)
{
	delete m_Points;
	m_Points = NULL;
	
	// préparation de la lecture du fichier
	MTTO_BufferInfo l_Buffer;
	l_Buffer.start = (const char*)data;
	l_Buffer.pointer = (const char*)data;
	l_Buffer.size = size;
	
	// recherche de la ligne "#N"
	unsigned int l_NumPoints = 0;
	BString l_Line;
	while(ReadLine(l_Line,&l_Buffer))
	{
		if ((l_Line[0] == '#') && (l_Line[1] == 'N'))
		{
			sscanf(l_Line.String()+2," %d",&l_NumPoints);
			break;
		}
	}
	
	// on ne continue que s'il y a des points à lire
	if (l_NumPoints != 0)
	{
		// allocation d'une zone pour stocker les nombres
		float *l_Coordinates = new float[l_NumPoints*3];
		float *l_PointerCoord = l_Coordinates;
		rgb_color *l_Colors = new rgb_color[l_NumPoints];
		rgb_color *l_PointerCol = l_Colors;
		
		// lecture des lignes
		for (unsigned int i=0; i<l_NumPoints; i++)
		{
			bool l_Valide = false;	// devient true quand on a lu
									// une ligne valide
			
			while(!l_Valide)
			{	
				// lecture d'une ligne					
				if (!ReadLine(l_Line,&l_Buffer))
					// on est arrivés trop vite à la fin du fichier
				{
					(new BAlert("3DMorph text loader","Error in file! (end of file reached too soon)",
						"Cancel",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
					delete[] l_Coordinates;
					delete[] l_Colors;
					return;
				}
			
				// est-ce une ligne valide?
				if ((l_Line[0] == '#') && (l_Line[1] == 'P'))
				{
					float red,green,blue;
					if ( sscanf(l_Line.String()+2," %f %f %f %f %f %f",
						l_PointerCoord,l_PointerCoord+1,l_PointerCoord+2,
						&red,&green,&blue) != 6)
					{
						// erreur de syntaxe: il n'y a pas 6 flottants après #P
						delete[] l_Coordinates;
						delete[] l_Colors;
						return;
					}
					
					l_PointerCoord+=3;
					l_PointerCol->red = (uint8)(red*255);
					l_PointerCol->green = (uint8)(green*255);
					(l_PointerCol++)->blue = (uint8)(blue*255);
					l_Valide = true;
				}
			}						
		}
		
		// on a lu tous les points
		m_Points = new MTPoints(l_NumPoints,l_Coordinates,l_Colors);
		delete[] l_Coordinates;
		delete[] l_Colors;
	}
	else //(l_NumPoints != 0)
		(new BAlert("3DMorph text loader","Invalid file!","Cancel",NULL,NULL,
			B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
}

MTPoints *MTTextObject::Update(float)
{
	if (m_Points == NULL)
		return new MTPoints;
	else
		return new MTPoints(*m_Points);
}

BView *MTTextObject::CreateView()
{
	return (m_ConfigView = new MTTO_ConfigView(this));
}

bool MTTextObject::ReadLine(BString &line, struct MTTO_BufferInfo *buffer)
{
	// calcul de l'adresse de la fin du buffer
	const char *l_End = buffer->start+buffer->size;
	
	// on cherche un début de ligne
	char c = *buffer->pointer;
	buffer->pointer++;
	while((c==0xd) || (c==0xa))
	{
		if (buffer->pointer<l_End)
		{
			c = *buffer->pointer;
			buffer->pointer++;
		}
		else
			return false;	// fin de fichier
	}
	
	// maintenant on cherche la fin de la ligne
	const char *l_LineStart = buffer->pointer-1;
	while((buffer->pointer < l_End) && (c!=0xd) && (c!=0xa))
	{
		c = *buffer->pointer;
		buffer->pointer++;
	}
	
	// on y est!
	line.SetTo(l_LineStart,buffer->pointer-l_LineStart-1);
	return true;
}	

MTTO_ConfigView::MTTO_ConfigView(MTTextObject *object)
	: BBox(BRect(0,0,350,60),NULL,B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP),
	  m_Object(object)
{
	SetLabel("3D Morph Text File Object");
	BRect l_ButtonRect(10,16,100,20);
	m_Button = new BButton(l_ButtonRect,NULL,"New File",
		new BMessage('ToNf'));
	AddChild(m_Button);
	ResizeTo(350,m_Button->Frame().bottom+10);
}

MTTO_ConfigView::~MTTO_ConfigView()
{
}

void MTTO_ConfigView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'ToNf':
		{
			BMessage l_Notification('ToFp');
			g_FilePanel->SetMessage(&l_Notification);
			
			BMessenger l_Me(this);
			g_FilePanel->SetTarget(l_Me);
			
			g_FilePanel->Show();
		
			break;
		}
		
		case 'ToFp':
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
					m_Object->RequestUpdate();
					
fin:				delete l_Buffer;
				}
				else
					(new BAlert("3DMorph text loader",
						"File must be a text file!","Cancel",NULL,NULL,
						B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
			}
			break;
		}
		
		default:
			BBox::MessageReceived(message);
			break;
	}
}

void MTTO_ConfigView::AttachedToWindow()
{
	m_Button->SetTarget(this);
	
	BBox::AttachedToWindow();
}		