/*
 * 3D Morph Toolkit
 *
 * (c) 2000 Sylvain Tertois
 *
 *
 * MTMatrix: class that make 3D linear transformations easier
**/

#include "MTMatrix.h"
#include <math.h>

MTMatrix::MTMatrix(float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l)
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

MTMatrix::MTMatrix()
{
	for (int i=0; i<12; i++)
		matrix[i]=0;
	matrix[0]=1;
	matrix[4]=1;
	matrix[8]=1;
}

MTMatrix &MTMatrix::operator=(const MTMatrix &source)
{
	for (int i=0; i<12; i++)
		matrix[i] = source.matrix[i];

	return *this;
}

MTMatrix &MTMatrix::operator*=(const MTMatrix &m2)
{
	MTMatrix m1(*this); // on crée une copie de la matrice actuelle
	
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

	return *this;
}

MTMatrix MTMatrix::operator*(const MTMatrix &m2) const
{
	MTMatrix res(*this); // on crée une copie de la matrice actuelle
	
/* lignes */
	for(int i=0; i<4; i++)
/* colonnes */
		for(int j=0; j<3; j++)
/* sommation */
		{
			res.Elem(i,j)=ElemConst(i,0)*m2.ElemConst(0,j);
			for(int k=1; k<3; k++)
				res.Elem(i,j)+=ElemConst(i,k)*m2.ElemConst(k,j);
		}
/* ajout des translations */
	for(int j=0; j<3; j++)
		res.Elem(3,j)+=m2.ElemConst(3,j);

	return res;
}

MTMatrix MTMatrix::Scale(float s)
{
	return MTMatrix(s,0,0,
					0,s,0,
					0,0,s,
					0,0,0);
}

MTMatrix MTMatrix::Scale(float x,float y,float z)
{
	return MTMatrix(x,0,0,
					0,y,0,
					0,0,z,
					0,0,0);
}

MTMatrix MTMatrix::Translate(float x,float y,float z)
{
	return MTMatrix(1,0,0,
					0,1,0,
					0,0,1,
					x,y,z);
}

MTMatrix MTMatrix::RotateX(float alpha)
{
	return MTMatrix(1          ,0          ,0          ,
					0          ,cos(alpha) ,sin(alpha) ,
					0          ,-sin(alpha),cos(alpha) ,
					0          ,0          ,0          );
}

MTMatrix MTMatrix::RotateY(float alpha)
{
	return MTMatrix(cos(alpha) ,0          ,-sin(alpha),
					0          ,1          ,0          ,
					sin(alpha) ,0          ,cos(alpha) ,
					0          ,0          ,0          );
}

MTMatrix MTMatrix::RotateZ(float alpha)
{
	return MTMatrix(cos(alpha) ,sin(alpha) ,0          ,
					-sin(alpha),cos(alpha) ,0          ,
					0          ,0          ,1          ,
					0          ,0          ,0          );
}
