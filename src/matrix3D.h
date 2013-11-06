/* librairie 3D
*  Commencée le 15 décembre 1998 par Sylvain Tertois
*
   matrices de transformation
*/

/* les matrices sont des "fausses 4*4": elles sont de la forme:
   a b c 0
   d e f 0
   g h i 0
   k l m 1
   
   la quatrième colonne est constante et n'est pas stoquée en mémoire.
   Les termes k l m servent à faire de la translation
*/

#ifndef _MATRIX3D_H
#define _MATRIX3D_H

class Matrix3D
{
public:
/* crée une matrice identité */
	Matrix3D();
	
private:
/* constucteurs privés */
// avec toutes les valeurs
	Matrix3D(float,float,float,float,float,float,float,float,float,float,float,float);

public:
	void operator=(const Matrix3D&);
	void operator*=(const Matrix3D&);
	
/* mise à l'échelle: soit Scale(f), soit Scale(x,y,z) 
     !!! Valeurs 0 interdites !!! */
	void Scale(float x,float y=0,float z=0);
/* translation */
	void Translate(float x,float y,float z);
/* rotations */
	void RotateX(float alpha);
	void RotateY(float alpha);
	void RotateZ(float alpha);
	
	friend void List3D::Transform(Matrix3D&);
	
/* pour débug */
	void Print();
	
private:
	inline const float &ElemConst(unsigned int i,unsigned int j) const { return (matrix[i*3+j]); }
	inline float &Elem(unsigned int i,unsigned int j) { return (matrix[i*3+j]); }
	float matrix[12];
};


#endif
