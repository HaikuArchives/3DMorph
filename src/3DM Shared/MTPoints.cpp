/*
 * 3D Morph Toolkit
 *
 * (c) 2000 Sylvain Tertois
 *
 *
 * MTPoints: class that store a group of points, with their colors
 */

#include "MTPoints.h"
#include "MTVector.h"
#include "MTMatrix.h"
#include <GraphicsDefs.h>
#include <Message.h>
#include <math.h>

MTPoints::MTPoints()
: m_NPoints(0), m_Coordinates(NULL), m_Colors(NULL)
{
}

MTPoints::MTPoints(const MTPoints &src)
: m_NPoints(0), m_Coordinates(NULL), m_Colors(NULL)
{
	SetTo(src);
}

MTPoints::MTPoints(const MTVector &x, const MTVector &y, const MTVector &z, const MTVector &r, const MTVector &g,const MTVector &b)
: m_NPoints(0), m_Coordinates(NULL), m_Colors(NULL)
{
	SetTo(x,y,z,r,g,b);
}

MTPoints::MTPoints(unsigned int nPoints, const float *coordinates, 
	const rgb_color *colors)
: m_NPoints(0), m_Coordinates(NULL), m_Colors(NULL)
{
	SetTo(nPoints,coordinates,colors);
}

MTPoints::~MTPoints()
{
	delete m_Coordinates;
	delete m_Colors;
}

MTPoints &MTPoints::SetTo(const MTPoints &src)
{
	// on efface les anciens tableaux
	delete[] m_Coordinates;
	m_Coordinates = NULL;
	delete[] m_Colors;
	m_Colors = NULL;
	
	// transfert de la source
	m_NPoints = src.m_NPoints;
	/// test "foolproof"
	if (m_NPoints == 0)
		return *this;
	
	// copie
	/// allocation des nouveau tableaux
	m_Coordinates = new float[m_NPoints*3];
	m_Colors = new rgb_color[m_NPoints];
	
	/// réalisation de la copie
	unsigned int j=0;
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Coordinates[j] = src.m_Coordinates[j]; j++;
		m_Coordinates[j] = src.m_Coordinates[j]; j++;
		m_Coordinates[j] = src.m_Coordinates[j]; j++;
		m_Colors[i] = src.m_Colors[i];
	}
	
	return *this;
}
	
MTPoints &MTPoints::SetTo(unsigned int nPoints, const float *coordinates, 
	const rgb_color *colors)
{
	// on efface les anciens tableaux
	delete[] m_Coordinates;
	m_Coordinates = NULL;
	delete[] m_Colors;
	m_Colors = NULL;
	
	// transfert de la source
	m_NPoints = nPoints;
	/// test "foolproof"
	if (m_NPoints == 0)
		return *this;
	
	// copie
	/// allocation des nouveau tableaux
	m_Coordinates = new float[m_NPoints*3];
	m_Colors = new rgb_color[m_NPoints];
	
	/// réalisation de la copie
	unsigned int j=0;
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Coordinates[j] = coordinates[j]; j++;
		m_Coordinates[j] = coordinates[j]; j++;
		m_Coordinates[j] = coordinates[j]; j++;
		m_Colors[i] = colors[i];
	}
	
	return *this;
}
	
MTPoints &MTPoints::SetTo(const MTVector &x, const MTVector &y, const MTVector &z, const MTVector &r, const MTVector &g,const MTVector &b)
{
	// on vérifie que tous les vecteurs ont la même taille
	unsigned int t = x.NumValues();
	if ((y.NumValues() != t) || (z.NumValues() != t) || (r.NumValues() != t) ||
		(g.NumValues() != t) || (b.NumValues() != t))
		return *this;
		
	// on efface les anciens tableaux
	delete[] m_Coordinates;
	m_Coordinates = NULL;
	delete[] m_Colors;
	m_Colors = NULL;
	
	// transfert de la source
	m_NPoints = t;
	/// test "foolproof"
	if (m_NPoints == 0)
		return *this;
	
	// copie
	/// allocation des nouveau tableaux
	m_Coordinates = new float[m_NPoints*3];
	m_Colors = new rgb_color[m_NPoints];
	
	/// réalisation de la copie
	unsigned int j=0;
	const float *xf = x.Values(), *yf = y.Values(), *zf = z.Values();
	const float *rf = r.Values(), *gf = g.Values(), *bf = b.Values();
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Coordinates[j++] = xf[i];
		m_Coordinates[j++] = yf[i];
		m_Coordinates[j++] = zf[i];
		m_Colors[i].red = (uint8)(rf[i]*255);
		m_Colors[i].green = (uint8)(gf[i]*255);
		m_Colors[i].blue = (uint8)(bf[i]*255);
	}
	
	return *this;
}

// fonctions d'archivage
/// noms des champs utilisés
static const char *mn_npoints = "NumPoints";
static const char *mn_coords = "Coords";
static const char *mn_colors = "Colors";
MTPoints::MTPoints(BMessage *archive)
: m_NPoints(0), m_Coordinates(NULL), m_Colors(NULL)
{
	int32 nPoints;
	const float *coords;
	const rgb_color *colors;
	ssize_t size;
	
	if ((archive->FindInt32(mn_npoints,&nPoints) == B_OK) &&
		(archive->FindData(mn_coords,B_FLOAT_TYPE,(const void**)&coords,&size) == B_OK) &&
		((unsigned int)size == nPoints*3*sizeof(float)) &&
		(archive->FindData(mn_colors,B_RGB_COLOR_TYPE,(const void**)&colors,&size) == B_OK) &&
		((unsigned int)size == nPoints*sizeof(rgb_color)))
	{
		SetTo((unsigned int)nPoints,coords,colors);
	}
}

BArchivable *MTPoints::Instantiate(BMessage *archive) 
{ 
	if ( !validate_instantiation(archive, "MTPoints") ) 
		return NULL; 
	return new MTPoints(archive); 
}

status_t MTPoints::Archive(BMessage *archive,bool deep) const
{
	archive->AddInt32(mn_npoints,NumPoints());
	archive->AddData(mn_coords,B_FLOAT_TYPE,(const void**)&m_Coordinates,
		NumPoints()*3*sizeof(float));
	archive->AddData(mn_colors,B_RGB_COLOR_TYPE,(const void**)&m_Colors,
		NumPoints()*sizeof(rgb_color));
	
	archive->AddString("class","MTPoints");
	
	return BArchivable::Archive(archive,deep);
}

MTPoints &MTPoints::Add(const MTVector &x,const MTVector &y,const MTVector &z,const MTVector &r,const MTVector &g,const MTVector &b)
{
	// on vérifie que tous les vecteurs ont la même taille
	unsigned int t = x.NumValues();
	if ((y.NumValues() != t) || (z.NumValues() != t) || (r.NumValues() != t) ||
		(g.NumValues() != t) || (b.NumValues() != t))
		return *this;

	// on alloue de nouvelles zones
	float *coords = new float[(m_NPoints+t)*3];
	rgb_color *colors = new rgb_color[m_NPoints+t];
	
	// coordonnées
	/// copie des anciens points
	for (unsigned int i=0; i<m_NPoints*3; i++)
	{
		coords[i] = m_Coordinates[i];
		colors[i] = m_Colors[i];
	}
	
	/// copie des coordonnées supplémentaires
	unsigned int j = m_NPoints*3;
	unsigned int k = m_NPoints;
	for (unsigned int i=0; i<t; i++)
	{
		coords[j++] = x.Values()[i];
		coords[j++] = y.Values()[i];
		coords[j++] = z.Values()[i];
		colors[k].red = (uint8)(r.Values()[i]*255);
		colors[k].green = (uint8)(g.Values()[i]*255);
		colors[k++].blue = (uint8)(b.Values()[i]*255);
	}
	
	// échange des anciens et nouveaux tableaux
	delete m_Coordinates;
	m_Coordinates = coords;
	delete m_Colors;
	m_Colors = colors;

	// mise à jour du nombre de points
	m_NPoints += t;
	
	return *this;
}

MTPoints &MTPoints::Add(const MTPoints &p)
{
	// on alloue de nouvelles zones
	float *coords = new float[(m_NPoints+p.m_NPoints)*3];
	rgb_color *colors = new rgb_color[m_NPoints+p.m_NPoints];
	
	// coordonnées
	/// copie des anciens points
	unsigned int j = 0;
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		coords[j] = m_Coordinates[j]; j++;
		coords[j] = m_Coordinates[j]; j++;
		coords[j] = m_Coordinates[j]; j++;
		colors[i] = m_Colors[i];
	}
	
	/// copie des coordonnées supplémentaires
	unsigned int k = m_NPoints;
	unsigned int l = 0;
	for (unsigned int i=0; i<p.m_NPoints; i++)
	{
		coords[j++] = p.m_Coordinates[l++];
		coords[j++] = p.m_Coordinates[l++];
		coords[j++] = p.m_Coordinates[l++];
		colors[k++] = p.m_Colors[i];
	}
	
	// échange des anciens et nouveaux tableaux
	delete m_Coordinates;
	m_Coordinates = coords;
	delete m_Colors;
	m_Colors = colors;
	
	// mise à jour du nombre de points
	m_NPoints += p.m_NPoints;

	return *this;
}

MTPoints MTPoints::operator&(const MTPoints &p) const
{
	MTPoints ret(*this);
	
	ret.Add(p);
	
	return ret;
}

bool MTPoints::SetColor(const MTPoints &src)
{
	// on teste le nombre de points
	if (m_NPoints != src.m_NPoints)
		return false;
	
	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints; i++)
		m_Colors[i] = src.m_Colors[i];
	
	return true;
}

bool MTPoints::SetColor(const MTVector &r, const MTVector &g, const MTVector &b)
{
	// on teste le nombre de points
	if ((m_NPoints != r.NumValues()) || (m_NPoints != g.NumValues()) ||
		(m_NPoints != g.NumValues()))
		return false;

	const float *rf = r.Values(), *gf = g.Values(), *bf = b.Values();

	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Colors[i].red = (uint8)(rf[i]*255);
		m_Colors[i].green = (uint8)(gf[i]*255);
		m_Colors[i].blue = (uint8)(bf[i]*255);
	}
	
	return true;
}

bool MTPoints::SetColor(float r, float g, float b)
{
	uint8 uir = (uint8)(r*255), uig = (uint8)(g*255), uib = (uint8)(b*255);

	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Colors[i].red = uir;
		m_Colors[i].green = uig;
		m_Colors[i].blue = uib;
	}
	
	return true;
}

bool MTPoints::SetColor(const rgb_color &color)
{
	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints; i++)
		m_Colors[i] = color;
	
	return true;
}

bool MTPoints::SetColor(float r1, float g1, float b1, float r2 ,float g2, float b2)
{
	float n = m_NPoints-1;
	
	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Colors[i].red = (uint8)(255*(r2*i/n+r1*(n-i)/n));
		m_Colors[i].green = (uint8)(255*(g2*i/n+g1*(n-i)/n));
		m_Colors[i].blue = (uint8)(255*(b2*i/n+b1*(n-i)/n));
	}
	
	return true;
}

bool MTPoints::SetColor(const rgb_color &c1, const rgb_color &c2)
{
	float n = m_NPoints-1;
	
	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Colors[i].red = (uint8)(((float)c2.red)*i/n+((float)c1.red)*(n-i)/n);
		m_Colors[i].green = (uint8)(((float)c2.green)*i/n+((float)c1.green)*(n-i)/n);
		m_Colors[i].blue = (uint8)(((float)c2.blue)*i/n+((float)c1.blue)*(n-i)/n);
	}
	
	return true;
}

bool MTPoints::SetCoordinates(const MTPoints &src)
{
	// on teste le nombre de points
	if (m_NPoints != src.m_NPoints)
		return false;

	// réalisation de la copie
	for (unsigned int i=0; i<m_NPoints*3; i++)
		m_Coordinates[i] = src.m_Coordinates[i];
	
	return true;
}

bool MTPoints::SetCoordinates(const MTVector &x, const MTVector &y, const MTVector &z)
{
	// on teste le nombre de points
	if ((m_NPoints != x.NumValues()) || (m_NPoints != y.NumValues()) ||
		(m_NPoints != z.NumValues()))
		return false;
		
	const float *xf = x.Values(), *yf = y.Values(), *zf = z.Values();

	// réalisation de la copie
	unsigned int j=0;
	for (unsigned int i=0; i<m_NPoints; i++)
	{
		m_Coordinates[j++] = xf[i];
		m_Coordinates[j++] = yf[i];
		m_Coordinates[j++] = zf[i];
	}

	return true;
}

MTPoints &MTPoints::Transform(const MTMatrix &mat)
{
	if ( m_NPoints == 0)
		return *this;
	
	float *pR=m_Coordinates;
	
/* lignes */
	for (unsigned int i=0; i<m_NPoints; i++)
	{
/* copie du point original + début de calcul des nouveaux points: 
                            on commence par mettre la translation */
		float pO[3];
		for (int j=0; j<3; j++)
		{
			pO[j]=pR[j];
			pR[j]=mat.ElemConst(3,j);
		}
/* calcul du produit matriciel */
		for (int j=0; j<3; j++)
			for (int k=0; k<3; k++)
				pR[j] += pO[k]*mat.ElemConst(k,j);
/* passage au point suivant */
		pR+=3;
	}
	
	return *this;
}	

MTPoints MTPoints::operator*(const MTMatrix &mat) const
{
	MTPoints res(*this);

	if ( m_NPoints == 0)
		return res;
	
	float *pR=res.m_Coordinates;
	const float *pO=m_Coordinates;
	
/* lignes */
	for (unsigned int i=0; i<m_NPoints; i++)
	{
/* copie du point original + début de calcul des nouveaux points: 
                            on commence par mettre la translation */
		for (int j=0; j<3; j++)
			pR[j]=mat.ElemConst(3,j);
/* calcul du produit matriciel */
		for (int j=0; j<3; j++)
			for (int k=0; k<3; k++)
				pR[j] += pO[k]*mat.ElemConst(k,j);
/* passage au point suivant */
		pR+=3;
		pO+=3;
	}
	
	return res;
}	

BPoint* MTPoints::Project(float depth,BPoint *proj) const
{
	if ( m_NPoints == 0 )
		return (NULL); 

	float *p = m_Coordinates;
	
	for (unsigned int i=0; i<m_NPoints; i++)
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

// Cette macro est une fonction destinée à remplacer le modulo:
//  au lieu de faire une dent de scie (//////) cette fonction fait un triangle (/\/\/\/\/\)
#define NMOD(x,y) (( x % (2*y))>=y?(y-1-(x%y)):x%y)
// Morphing
MTPoints MTPoints::Morphing(MTPoints &p1, MTPoints &p2, float f)
{
	if (f==0)
		return p1;
	else if (f==1)
		return p2;
	MTPoints result;

// nombre de points à calculer
	unsigned int n1 = p1.NumPoints();
	unsigned int n2 = p2.NumPoints();
	unsigned int nR = n1;
	if (n2 > nR)
		nR = n2;
	// tests 'foolproof'
	if (nR == 0)
		return result;
	if (n1 == 0)
		return p2;
	if (n2 == 0)
		return p1;
	
// reservation des objets pour le résultat 
	result.m_NPoints = nR;
	result.m_Coordinates = new float[3*nR];
	result.m_Colors = new rgb_color[nR];
	
// calcul des nouveaux points
/// on regarde s'il faudra faire des tests de débordements pour les couleurs
	if ((f<0) || (f>1))
	{
		/// version avec tests
		for (unsigned int i=0; i<nR; i++)
		{
			unsigned int i1 = NMOD(i,n1);
			unsigned int i2 = NMOD(i,n2);

			result.m_Coordinates[i*3]=p1.m_Coordinates[i1*3]*(1-f)+p2.m_Coordinates[i2*3]*f;
			result.m_Coordinates[i*3+1]=p1.m_Coordinates[i1*3+1]*(1-f)+p2.m_Coordinates[i2*3+1]*f;
			result.m_Coordinates[i*3+2]=p1.m_Coordinates[i1*3+2]*(1-f)+p2.m_Coordinates[i2*3+2]*f;

			float color = p1.m_Colors[i1].red*(1-f)+p2.m_Colors[i2].red*f;
			if (color<0)
				result.m_Colors[i].red = 0;
			else if (color>255)
				result.m_Colors[i].red = 255;
			else
				result.m_Colors[i].red = (uint8)color;
				
			color = p1.m_Colors[i1].green*(1-f)+p2.m_Colors[i2].green*f;
			if (color<0)
				result.m_Colors[i].green = 0;
			else if (color>255)
				result.m_Colors[i].green = 255;
			else
				result.m_Colors[i].green = (uint8)color;
			
			color = p1.m_Colors[i1].blue*(1-f)+p2.m_Colors[i2].blue*f;
			if (color<0)
				result.m_Colors[i].blue = 0;
			else if (color>255)
				result.m_Colors[i].blue = 255;
			else
				result.m_Colors[i].blue = (uint8)color;			
		}
	}
	else
	{
		/// version sans tests
		for (unsigned int i=0; i<nR; i++)
		{
			unsigned int i1 = NMOD(i,n1);
			unsigned int i2 = NMOD(i,n2);

			result.m_Coordinates[i*3]=p1.m_Coordinates[i1*3]*(1-f)+p2.m_Coordinates[i2*3]*f;
			result.m_Coordinates[i*3+1]=p1.m_Coordinates[i1*3+1]*(1-f)+p2.m_Coordinates[i2*3+1]*f;
			result.m_Coordinates[i*3+2]=p1.m_Coordinates[i1*3+2]*(1-f)+p2.m_Coordinates[i2*3+2]*f;

			result.m_Colors[i].red = (uint8)(p1.m_Colors[i1].red*(1-f)+p2.m_Colors[i2].red*f);
			result.m_Colors[i].green = (uint8)(p1.m_Colors[i1].green*(1-f)+p2.m_Colors[i2].green*f);
			result.m_Colors[i].blue = (uint8)(p1.m_Colors[i1].blue*(1-f)+p2.m_Colors[i2].blue*f);
		}
	}
	
	return result;
}

MTPoints MTPoints::Segment(float x1,float y1, float z1, float x2, float y2, float z2, unsigned int nPoints)
{
	MTVector color = MTVector::Constant(1,nPoints);
	return MTPoints(MTVector::GradientInc(x1,x2,nPoints),
		MTVector::GradientInc(y1,y2,nPoints),
		MTVector::GradientInc(z1,z2,nPoints),
		color,color,color);
}

MTPoints MTPoints::Circle(unsigned int nPoints)
{
	MTVector color = MTVector::Constant(1,nPoints);
	MTVector x = MTVector::GradientExc(0,2*PI,nPoints);
	MTVector y = sin(x);
	x.cos();
	
	return MTPoints(x,y,color*0,color,color,color);
}

	