/*
 * 3D Morph Toolkit
 *
 * (c) 2000 Sylvain Tertois
 *
 *
 * MTVector: float vector
**/

#include "MTVector.h"
#include <math.h>
#include <stdlib.h>

MTVector::MTVector()
	: m_NumValues(0), m_Values(NULL)
{
}

MTVector::MTVector(unsigned int nValues, const float *values)
	: m_NumValues(0), m_Values(NULL)
{
	SetTo(nValues,values);
}

MTVector::MTVector(const MTVector &src)
	: m_NumValues(0), m_Values(NULL)
{
	SetTo(src);
}

MTVector::~MTVector()
{
	delete[] m_Values;
}

MTVector &MTVector::SetTo(const MTVector &src)
{
	delete[] m_Values;
	
	m_NumValues = src.m_NumValues;

	// test "foolproof"
	if (src.m_NumValues == 0)
	{
		m_Values = NULL;
		return *this;
	}
	
	// allocation d'une nouvelle zone
	m_Values = new float[src.m_NumValues];
	
	// copie des données
	for (unsigned int i=0; i<src.m_NumValues; i++)
		m_Values[i] = src.m_Values[i];
	
	return *this;
}

MTVector &MTVector::SetTo(unsigned int nValues, const float *values)
{
	delete[] m_Values;

	m_NumValues = nValues;

	// test "foolproof"
	if (nValues == 0)
	{
		m_Values = NULL;
		return *this;
	}

	// allocation d'une nouvelle zone
	m_Values = new float[nValues];
	
	// copie des données
	for (unsigned int i=0; i<nValues; i++)
		m_Values[i] = values[i];
	
	return *this;
}
	
MTVector &MTVector::AddValues(const MTVector &src)
{
	// test "foolproof"
	if (src.m_NumValues == 0)
		return *this;
		
	// allocation d'une nouvelle zone
	float *newValues = new float[m_NumValues+src.m_NumValues];
	
	// copie des données
	for (unsigned int i=0; i<m_NumValues; i++)
		newValues[i] = m_Values[i];
	for (unsigned int i=0; i<src.m_NumValues; i++)
		newValues[i+m_NumValues] = src.m_Values[i];
	
	// échange des buffers
	delete m_Values;
	m_Values = newValues;
	
	m_NumValues += src.m_NumValues;

	return *this;
}

MTVector MTVector::operator&(const MTVector &src) const
{
	// création d'un nouveau vecteur
	MTVector newVector;
	/// test "foolproof"
	if (m_NumValues+src.m_NumValues == 0)
		return newVector;
	/// allocation de la zone mémoire
	newVector.m_Values = new float[m_NumValues+src.m_NumValues];

	// copie des données
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i];
	for (unsigned int i=0; i<src.m_NumValues; i++)
		newVector.m_Values[i+m_NumValues] = src.m_Values[i];
	
	newVector.m_NumValues = m_NumValues+src.m_NumValues;
	
	return newVector;
}
	
MTVector &MTVector::AddValue(float src)
{
	// allocation d'une nouvelle zone
	float *newValues = new float[m_NumValues+1];
	
	// copie des données
	for (unsigned int i=0; i<m_NumValues; i++)
		newValues[i] = m_Values[i];
	newValues[m_NumValues] = src;
	
	// échange des buffers
	delete m_Values;
	m_Values = newValues;
	
	m_NumValues++;

	return *this;
}

MTVector MTVector::operator&(float src) const
{
	// création d'un nouveau vecteur
	MTVector newVector;
	newVector.m_Values = new float[m_NumValues+1];

	// copie des données
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i];
	newVector.m_Values[m_NumValues] = src;
	
	newVector.m_NumValues = m_NumValues+1;
	
	return newVector;
}

void MTVector::Repeat(unsigned int each, unsigned int list)
{
	// calcul de la nouvelle taille
	unsigned int newSize = m_NumValues*each*list;
	
	// test "foolproof"
	if (newSize==0)
	{
		m_NumValues = newSize;
		delete[] m_Values;
		m_Values = NULL;
	}
	else
	{
		// allocation d'une nouvelle zone
		float *newValues = new float[newSize];
		
		// réalisation de la copie
		unsigned int index = 0;
		/// boucle par nombre de copies de liste
		for (unsigned int nlists=0; nlists<list; nlists++)
			/// boucle dans chaque élément de la liste
			for (unsigned int element=0; element<m_NumValues; element++)
				// boucle pour copier chaque élément
				for (unsigned int i=0; i<each; i++)
					newValues[index++] = m_Values[element];
	
		// échange du vieux et du nouveau vecteur
		delete[] m_Values;
		m_Values = newValues;
		m_NumValues = newSize;
	}
}
					
MTVector &MTVector::operator+=(const MTVector &src)
{
	if (m_NumValues != src.m_NumValues)
		return *this;
		
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] += src.m_Values[i];

	return *this;
}

MTVector MTVector::operator+(const MTVector &src) const
{
	if (m_NumValues != src.m_NumValues)
		return *this;
	
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]+src.m_Values[i];
	
	return newVector;
}

MTVector &MTVector::operator-=(const MTVector &src)
{
	if (m_NumValues != src.m_NumValues)
		return *this;
		
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] -= src.m_Values[i];

	return *this;
}

MTVector MTVector::operator-(const MTVector &src) const
{
	if (m_NumValues != src.m_NumValues)
		return *this;
	
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]-src.m_Values[i];
	
	return newVector;
}

MTVector &MTVector::operator*=(const MTVector &src)
{
	if (m_NumValues != src.m_NumValues)
		return *this;
		
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] *= src.m_Values[i];

	return *this;
}

MTVector MTVector::operator*(const MTVector &src) const
{
	if (m_NumValues != src.m_NumValues)
		return *this;
	
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]*src.m_Values[i];
	
	return newVector;
}

MTVector &MTVector::operator/=(const MTVector &src)
{
	if (m_NumValues != src.m_NumValues)
		return *this;
		
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] /= src.m_Values[i];

	return *this;
}

MTVector MTVector::operator/(const MTVector &src) const
{
	if (m_NumValues != src.m_NumValues)
		return *this;
	
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]/src.m_Values[i];
	
	return newVector;
}

MTVector &MTVector::operator+=(float src)
{
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] += src;

	return *this;
}

MTVector MTVector::operator+(float src) const
{
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]+src;
	
	return newVector;
}

MTVector &MTVector::operator-=(float src)
{
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] -= src;

	return *this;
}

MTVector MTVector::operator-(float src) const
{
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]-src;
	
	return newVector;
}

MTVector &MTVector::operator*=(float src)
{
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] *= src;

	return *this;
}

MTVector MTVector::operator*(float src) const
{
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]*src;
	
	return newVector;
}

MTVector &MTVector::operator/=(float src)
{
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] /= src;

	return *this;
}

MTVector MTVector::operator/(float src) const
{
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return *this;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
		newVector.m_Values[i] = m_Values[i]/src;
	
	return newVector;
}

void MTVector::sin()
{
	// boucle dans les valeurs
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] = sinf(m_Values[i]);
}

void MTVector::cos()
{
	// boucle dans les valeurs
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] = cosf(m_Values[i]);
}

void MTVector::tan()
{
	// boucle dans les valeurs
	for (unsigned int i=0; i<m_NumValues; i++)
		m_Values[i] = tanf(m_Values[i]);
}

static float dummy;
float &MTVector::operator[](unsigned int index) const
{
	if (m_NumValues==0)
		return dummy;
		
	if (index>=m_NumValues)
		index=m_NumValues-1;
	
	return(m_Values[index]);
}

MTVector MTVector::operator^(const MTVector &src) const
{
	// allocation d'un nouveau vecteur
	MTVector newVector;
	newVector.m_NumValues = m_NumValues;
	/// test "foolproof"
	if (m_NumValues == 0)
		return newVector;
	/// allocation elle-même
	newVector.m_Values = new float[m_NumValues];
	
	// réalisation de l'opération
	for (unsigned int i=0; i<m_NumValues; i++)
	{
		int v = (unsigned int)m_Values[i];
		v %= src.m_NumValues;
		if (v<0)
			v+= src.m_NumValues;
			
		newVector.m_Values[i] = src.m_Values[v];
	}
	
	return newVector;
}

MTVector sin(const MTVector &src)
{
	MTVector ret(src);
	ret.sin();
	
	return ret;
}

MTVector cos(const MTVector &src)
{
	MTVector ret(src);
	ret.cos();
	
	return ret;
}

MTVector tan(const MTVector &src)
{
	MTVector ret(src);
	ret.tan();
	
	return ret;
}

MTVector MTVector::Constant(float constant, unsigned int nValues)
{
	// création du vecteur
	MTVector ret;
	
	if (nValues>0)
	{
		ret.m_NumValues = nValues;
		ret.m_Values = new float[nValues];
		
		// remplissage du vecteur
		for (unsigned int i=0; i<nValues; i++)
			ret.m_Values[i] = constant;
	}
	
	return ret;
}

MTVector MTVector::GradientInc(float val1, float val2, unsigned int nValues)
{
	// création du vecteur
	MTVector ret;
	
	if (nValues>0)
	{
		ret.m_NumValues = nValues;
		ret.m_Values = new float[nValues];
		
		// remplissage du vecteur
		for (unsigned int i=0; i<nValues; i++)
			ret.m_Values[i] = val1*(1-((float)i)/(nValues-1))+val2*(((float)i)/(nValues-1));
	}
	
	return ret;
}

MTVector MTVector::GradientExc(float val1, float val2, unsigned int nValues)
{
	// création du vecteur
	MTVector ret;
	
	if (nValues>0)
	{
		ret.m_NumValues = nValues;
		ret.m_Values = new float[nValues];
		
		// remplissage du vecteur
		for (unsigned int i=0; i<nValues; i++)
			ret.m_Values[i] = val1*(1-((float)i)/(nValues-1))+val2*(((float)i)/nValues);
	}
	
	return ret;
}

MTVector MTVector::Random(float val1, float val2, unsigned int nValues)
{
	// création du vecteur
	MTVector ret;
	
	if (nValues>0)
	{
		ret.m_NumValues = nValues;
		ret.m_Values = new float[nValues];
		
		// remplissage du vecteur
		for (unsigned int i=0; i<nValues; i++)
			ret.m_Values[i] = ((float)rand()/RAND_MAX)*(val2-val1)+val1;
	}
	
	return ret;
}