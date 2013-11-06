/* librairie 3D
 *
 *  morphing entre deux objets 3D
 *
 * commencé le 30 décembre 1998 par Sylvain Tertois
*/

#ifndef _MORPHING3D_H
#define _MORPHING3D_H

typedef enum { M3D_UNIFORM, M3D_PROGRESSIVE } MorphingType;

extern void Morphing(MorphingType t,float f,List3D *obj1,rgb_color *col1,List3D *obj2,rgb_color *col2,List3D **objRes,rgb_color **colRes);
	/* si objRes et colRes sont non nuls, ils sont libérés avant d'être réalloués.
	   l'utilisateur n'a pas à s'occuper de leur allocation.
	   f est un nombre entre 0 et 1: si f=0, on a obj1, si f=1, on a obj2 */
	   
#endif