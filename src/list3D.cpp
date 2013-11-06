/* librairie 3D
*  Commencée le 15 décembre 1998 par Sylvain Tertois
*  Terminée le 19 décembre 1998
*
   liste de points 3D
*/

#include "list3D.h"
#include "matrix3D.h"

#include <Point.h>


List3D::List3D(unsigned int s)
{
	numPoints = s;
	points = new float[s*3];
}

List3D::List3D(unsigned int size, float *coords)
{
	numPoints = size;
	points = coords;
}

List3D::~List3D()
{
	delete points;
}

List3D::List3D(List3D &source)
{
	numPoints = source.numPoints;
	if  (source.numPoints==0)
		points = NULL;
	else
	{	
		points = new float[numPoints*3];
	
		for (unsigned int i=0; i<numPoints*3; i++)
			points[i]=source.points[i];
	}
}

List3D::List3D()
{
	numPoints = 0;
	points = NULL;
}

status_t List3D::PutPoint(unsigned int pointNum, float x, float y, float z)
{
	if (pointNum >= numPoints)
		return(B_BAD_INDEX);
	float *p = &points[pointNum*3];
	
	*p++ = x;
	*p++ = y;
	*p++ = z;
	return (B_OK);
}

BPoint* List3D::Project(float depth,BPoint *proj)
{
	if ( numPoints == 0 )
		return (NULL); 

	float *p = points;
	
	for (unsigned int i=0; i<numPoints; i++)
	{
/* calcul du facteur de proportionalité 
  f = d/(d+z) */
		float f = depth/(depth+p[2]);
/* le point projeté a alors comme coords f*x et f*y */
		proj[i].Set( f*p[0], f*p[1] );
		p+=3;
	}
	return (proj);
}

void List3D::Transform(Matrix3D &mat)
{
	if ( numPoints == 0)
		return;
	
	float *pR=points;
	
/* lignes */
	for (unsigned int i=0; i<numPoints; i++)
	{
/* copie du point original + début de calcul des nouveaux points: 
                            on commence par mettre la translation */
		float pO[3];
		for (int j=0; j<3; j++)
		{
			pO[j]=pR[j];
			pR[j]=mat.Elem(3,j);
		}
/* calcul du produit matriciel */
		for (int j=0; j<3; j++)
			for (int k=0; k<3; k++)
				pR[j] += pO[k]*mat.Elem(k,j);
/* passage au point suivant */
		pR+=3;
	}
}	

