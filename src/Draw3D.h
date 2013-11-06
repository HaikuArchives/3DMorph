/* écran de veille 3dMorph */

/* classe pour le dessin des points 3D */

typedef	short int15;

#include <Point.h>
#include <Region.h>
#include <Screen.h>
#include "list3D.h"
#include <GraphicsCard.h>

class Draw3D
{
public:
// données sur la taille de l'écran
	int height,width,objectSize;
// points 2D affichés
	BPoint *pVoldPoints,*pOldPoints,*pNewPoints;	// Vold = Very old
// couleur des points affichés
	rgb_color *pointsColor;
// nombre de points 2D dans les tableaux pOldPoints et pNewPoints
	unsigned int numVoldPoints,numOldPoints,numNewPoints;
// paramètres DirectWindow
	clipping_rect *dwClipList;	// liste de clipping
	int dwClipNum;				// nombre de rectangles de clipping
	uint32 *dwBits;				// pointeur vers la mémoire vidéo
	int32 dwRowBytes;			// octets par ligne
	color_space dwFormat;		// types de mapping (8bits, 16bis ou 32bits)
	bool *dwDoClip;				// tableau de booléens, un par point (clipping ou non?)
	bool *dwOldDoClip;			// le même en plus vieux
	int32 dwtx,dwty;			// translation par rapport à l'origine
// paramètres d'affichage
	bool doubleBuffering,softwareDouble;
	int32 frameNum;
	float depth;				// paramètre d'effet de profondeur (différence de luminosité suivant la distance)
	bool m_NiceDraw;			// dessin en 5 ou 9 pixels/point
// coords des points 3D affichés en ce moment, et celles des 2 objets dont on fait la morphing
	List3D	*points3D;
// vue utilisée pour l'écran de veille
	bool preview;
// palette de couleurs pour les modes 8 bits
	const uint8 *map;

// fonctions de conversion R,V et B vers un écran
	inline void convert(uint8 red, uint8 green, uint8 blue, uint8 *el) 
		{ *el = map[((red & 0xf8)<<7) | ((green & 0xf8)<<2) | ((blue & 0xf8) >> 3)];}
	inline void convert(uint8 red, uint8 green, uint8 blue, int15 *el) 
		{ *el = (((red & 0xf8)<<7) | ((green & 0xf8)<<2) | ((blue & 0xf8)>> 3));}
	inline void convert(uint8 red, uint8 green, uint8 blue, uint16 *el) 
		{ *el = (((red & 0xf8)<<8) | ((green & 0xfc)<<3) | ((blue & 0xf8)>> 3));}
	inline void convert(uint8 red, uint8 green, uint8 blue, uint32 *el) 
		{ *el = (red<<16) | (green<<8) | (blue);}
	
	inline uint8 Max(uint8 p1,uint8 p2)
		{ return (p1); }
	inline int15 Max(int15 p1, int15 p2)
		{ return ( (((p1&0x7c00)>(p2&0x7c00))?(p1&0x7c00):(p2&0x7c00)) |
				   (((p1&0x03e0)>(p2&0x03e0))?(p1&0x03e0):(p2&0x03e0)) |
				   (((p1&0x001f)>(p2&0x001f))?(p1&0x001f):(p2&0x001f))); }
	inline uint16 Max(uint16 p1, uint16 p2)
		{ return ( (((p1&0xf800)>(p2&0xf800))?(p1&0xf800):(p2&0xf800)) |
				   (((p1&0x07e0)>(p2&0x07e0))?(p1&0x07e0):(p2&0x07e0)) |
				   (((p1&0x001f)>(p2&0x001f))?(p1&0x001f):(p2&0x001f))); }
	inline uint32 Max(uint32 p1, uint32 p2)
		{ return ( (((p1&0x00ff0000)>(p2&0x00ff0000))?(p1&0x00ff0000):(p2&0x00ff0000)) |
				   (((p1&0x0000ff00)>(p2&0x0000ff00))?(p1&0x0000ff00):(p2&0x0000ff00)) |
				   (((p1&0x000000ff)>(p2&0x000000ff))?(p1&0x000000ff):(p2&0x000000ff))); }
		
	// fonctions de dessin
	// nom: DoDraw3dMorphXXY où XX est le nombre de bits par pixel, Y est le nombre
	// de pixels par point
	void DoDraw3dMorph085();
	void DoDraw3dMorph155();
	void DoDraw3dMorph165();
	void DoDraw3dMorph325();
	void DoDraw3dMorph089();
	void DoDraw3dMorph159();
	void DoDraw3dMorph169();
	void DoDraw3dMorph329();
	
	// fonction hook pour effacer les points
#ifdef _USE_HOOK
	graphics_card_hook m_DrawRectHook;
	typedef int32 (*sync_hook)(void);
	sync_hook m_SyncHook;
#endif		
};
