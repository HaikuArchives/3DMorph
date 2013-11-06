/* librairie 3D
*  Commencée le 15 décembre 1998 par Sylvain Tertois
*
   liste de points 3D
*/

#ifndef _LIST3D_H
#define _LIST3D_H

class Matrix3D;
class ResourceCreator;

#include <SupportKit.h>

class List3D
{
public:
	List3D(unsigned int size);
	List3D(unsigned int size,float *coords);
	List3D(List3D&);
	List3D();
	~List3D();
	
	inline unsigned int NumPoints() { return numPoints; }
	status_t PutPoint(unsigned int pointNum, float x, float y, float z);
	void Transform(Matrix3D&);
	BPoint* Project(float depth,BPoint *proj);

	float *points;

private:
	unsigned int numPoints;
};

#endif