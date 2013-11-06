/* librairie 3D
 *
 *  morphing entre deux objets 3D
 *
 * commencé le 30 décembre 1998 par Sylvain Tertois
*/


#include "list3D.h"
#include "morphing3D.h"

// Cette macro est une fonction destinée à remplacer le modulo:
//  au lieu de faire une dent de scie (//////) cette fonction fait un triangle (/\/\/\/\/\)
#define abs(x) (x>=0?x:-x)
//#define NMOD(x,y) (y - abs( (x % (2*y)) - y))
#define NMOD(x,y) (( x % (2*y))>=y?(y-1-(x%y)):x%y)

void Morphing(MorphingType t,float f,List3D *obj1,rgb_color *col1,List3D *obj2,rgb_color *col2,List3D **objRes,rgb_color **colRes)
{
/* nombre de points à calculer */
	unsigned int n1 = obj1->NumPoints();
	unsigned int n2 = obj2->NumPoints();
	unsigned int nR = n1;
	if (n2 > nR)
		nR = n2;
	
/* reservation des objets pour le résultat */
/* si la liste est déjà allouée et a la bonne taille, pas besoin de le faire */
	if ( (*objRes == NULL) || ((*objRes)->NumPoints() != nR ) )
	{
		if (*objRes != NULL)
			delete *objRes;
		if (*colRes != NULL)
			delete *colRes;
			*objRes = new List3D(nR);
			*colRes = new rgb_color[nR];
	}
	
/* calcul des nouveaux points */
	switch (t)
	{
		case M3D_UNIFORM:
		for (unsigned int i=0; i<nR; i++)
		{
			unsigned int i1 = NMOD(i,n1);
			unsigned int i2 = NMOD(i,n2);

			(*objRes)->points[i*3]=obj1->points[i1*3]*(1-f)+obj2->points[i2*3]*f;
			(*objRes)->points[i*3+1]=obj1->points[i1*3+1]*(1-f)+obj2->points[i2*3+1]*f;
			(*objRes)->points[i*3+2]=obj1->points[i1*3+2]*(1-f)+obj2->points[i2*3+2]*f;

			(*colRes)[i].red = (uint8)(col1[i1].red*(1-f)+col2[i2].red*f);
			(*colRes)[i].green = (uint8)(col1[i1].green*(1-f)+col2[i2].green*f);
			(*colRes)[i].blue = (uint8)(col1[i1].blue*(1-f)+col2[i2].blue*f);
		}
		break;
	
		case M3D_PROGRESSIVE:
		for (unsigned int i=0; i<nR; i++)
		{
			unsigned int i1 = NMOD(i,n1);
			unsigned int i2 = NMOD(i,n2);
			float newF = f*2-((float)i)/nR;
			newF = (newF<0.0)?0.0:newF;
			newF = (newF>1.0)?1.0:newF;

			(*objRes)->points[i*3]=obj1->points[i1*3]*(1-newF)+obj2->points[i2*3]*newF;
			(*objRes)->points[i*3+1]=obj1->points[i1*3+1]*(1-newF)+obj2->points[i2*3+1]*newF;
			(*objRes)->points[i*3+2]=obj1->points[i1*3+2]*(1-newF)+obj2->points[i2*3+2]*newF;

			(*colRes)[i].red = (uint8)(col1[i1].red*(1-newF)+col2[i2].red*newF);
			(*colRes)[i].green = (uint8)(col1[i1].green*(1-newF)+col2[i2].green*newF);
			(*colRes)[i].blue = (uint8)(col1[i1].blue*(1-newF)+col2[i2].blue*newF);
		}
		break;
	}		
}

	