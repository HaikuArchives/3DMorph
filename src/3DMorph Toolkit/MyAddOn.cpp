/*
 * 3D Morph Toolkit
 *
 *
 * Add-on
 *
 * main file
 *
 */

#include "MyAddOn.h"

#include "MTAddOn.h"
#include "MTVector.h"
#include "MTPoints.h"
#include "MTMatrix.h"

#include "resource3D.h"

#include "SSObjectView.h"

#include <String.h>
#include <Path.h>
#include <File.h>
#include <Alert.h>
#include <FindDirectory.h>

// Add-on exported symbols
/// add-on type
MTAddOnType addOnType = MT_TOOLKIT;

/// object instantiation function
MTObject *InstantiateObject()
{
	// create a new object
	return new MyAddOn;
}

// ***
// *** MyAddOn class
// ***

// constructor
MyAddOn::MyAddOn()
{
	m_Resource = NULL;
	m_ObjectNum = 0;
	
	// recherche de l'écran de veille
	BPath l_SaverPath;
	find_directory(B_USER_ADDONS_DIRECTORY,&l_SaverPath);
	l_SaverPath.Append("Screen Savers/3dMorph");
	
	BFile l_SaverFile;
	if (l_SaverFile.SetTo(l_SaverPath.Path(),B_READ_ONLY) != B_OK)
	{
		BString l_Message = "Failed to find 3DMorph.\nIt should be at ";
		l_Message << l_SaverPath.Path();
		
		(new BAlert("3DMorph Toolkit",l_Message.String(),"Cancel",NULL,NULL,
			B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
	}
	else
		// initialisation de l'objet resources
		m_Resource = new Resource3D(&l_SaverFile);
}

// destructor
MyAddOn::~MyAddOn()
{
	delete m_Resource;
}

// generate a new set of points
// musn't return NULL!
MTPoints *MyAddOn::Update(float)
{
	if (m_Resource == NULL)
		return new MTPoints;
	
	MTPoints *l_Points = m_Resource->GetData(m_ObjectNum);
	if (l_Points == NULL)
		return new MTPoints;
	else
		return l_Points;	
}

// creates a configuration view
// NULL if no configuration
BView *MyAddOn::CreateView()
{
	return new SSObjectView(this);
}