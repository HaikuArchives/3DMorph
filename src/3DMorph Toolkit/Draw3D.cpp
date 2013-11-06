/* écran de veille 3dMorph */

/* classe pour le dessin des points 3D */

#include "Draw3D.h"
#include "MTPoints.h"

// macro pour accéder à la coordonnée z d'un point
#define POINTZ(i) (points3D->Coordinates()[3*i+2]/objectSize)

// macro pour déclarer une fonction
#define DECL(x,y) void Draw3D::DoDraw3dMorph ## x ## y ## ()

// et c'est parti pour une scéance de copier-coller ;-)
#define FUNCTION void Draw3D::DoDraw3dMorph085()
#define NPIXELS 5
#define TYPEPIXEL uint8
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph155()
#define NPIXELS 5
#define TYPEPIXEL int15
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph165()
#define NPIXELS 5
#define TYPEPIXEL uint16
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph325()
#define NPIXELS 5
#define TYPEPIXEL uint32
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph089()
#define NPIXELS 9
#define TYPEPIXEL uint8
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph159()
#define NPIXELS 9
#define TYPEPIXEL int15
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph169()
#define NPIXELS 9
#define TYPEPIXEL uint16
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL

#define FUNCTION void Draw3D::DoDraw3dMorph329()
#define NPIXELS 9
#define TYPEPIXEL uint32
#include "Draw3DFunction.h"
#undef FUNCTION
#undef NPIXELS
#undef TYPEPIXEL
