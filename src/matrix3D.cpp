/* librairie 3D
*  Commencée le 15 décembre 1998 par Sylvain Tertois
*  Terminée le 19 décembre (par la même personne ;-)
*
   liste de points 3D
*/


#include "list3D.h"
#include "matrix3D.h"
#include <math.h>

Matrix3D::Matrix3D(float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l)
{
	matrix[0]=a;
	matrix[1]=b;
	matrix[2]=c;
	matrix[3]=d;
	matrix[4]=e;
	matrix[5]=f;
	matrix[6]=g;
	matrix[7]=h;
	matrix[8]=i;
	matrix[9]=j;
	matrix[10]=k;
	matrix[11]=l;
}

Matrix3D::Matrix3D()
{
	for (int i=0; i<12; i++)
		matrix[i]=0;
	matrix[0]=1;
	matrix[4]=1;
	matrix[8]=1;
}

void Matrix3D::operator=(const Matrix3D &source)
{
	for (int i=0; i<12; i++)
		matrix[i] = source.matrix[i];
}

void Matrix3D::operator*=(const Matrix3D &m2)
{
	Matrix3D m1(*this); // on crée une copie de la matrice actuelle
	
/* lignes */
	for(int i=0; i<4; i++)
/* colonnes */
		for(int j=0; j<3; j++)
/* sommation */
		{
			Elem(i,j)=m1.ElemConst(i,0)*m2.ElemConst(0,j);
			for(int k=1; k<3; k++)
				Elem(i,j)+=m1.ElemConst(i,k)*m2.ElemConst(k,j);
		}
/* ajout des translations */
	for(int j=0; j<3; j++)
		Elem(3,j)+=m2.ElemConst(3,j);
}

void Matrix3D::Scale(float x,float y,float z)
{
	if (y==0)
		y=x;
	if (z==0)
		z=x;
	(*this)*=Matrix3D(x,0,0,
	                  0,y,0,
	                  0,0,z,
	                  0,0,0);
}

void Matrix3D::Translate(float x,float y,float z)
{
	(*this)*=Matrix3D(1,0,0,
	                  0,1,0,
	                  0,0,1,
	                  x,y,z);
}

void Matrix3D::RotateX(float alpha)
{
	(*this)*=Matrix3D(1          ,0          ,0          ,
	                  0          ,cos(alpha) ,sin(alpha) ,
	                  0          ,-sin(alpha),cos(alpha) ,
	                  0          ,0          ,0          );
}

void Matrix3D::RotateY(float alpha)
{
	(*this)*=Matrix3D(cos(alpha) ,0          ,-sin(alpha),
	                  0          ,1          ,0          ,
	                  sin(alpha) ,0          ,cos(alpha) ,
	                  0          ,0          ,0          );
}

void Matrix3D::RotateZ(float alpha)
{
	(*this)*=Matrix3D(cos(alpha) ,sin(alpha) ,0          ,
	                  -sin(alpha),cos(alpha) ,0          ,
	                  0          ,0          ,1          ,
	                  0          ,0          ,0          );
}

void Matrix3D::Print()
{
	for ( int i=0; i<4; i++)
	{
		for ( int j=0; j<3; j++)
			printf("%f,\t",Elem(i,j));
		if ( i==3 )
			printf("1\n");
		else
			printf("0\n");
	}
}
