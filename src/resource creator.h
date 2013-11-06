/* créateur de resources pour mon écran de veille
 *
 * commencé le 28 décembre 1998 par Sylvain Tertois
**/

#ifndef _RESOURCE_CREATOR_H
#define _RESOURCE_CREATOR_H

#include "AppKit.h"
#include "StorageKit.h"
class ResourceCreator : public BApplication
{
	public:
		ResourceCreator();
		~ResourceCreator();
		
		void LoadObjects();
		void SaveResource();
		
	private:
		BList objects;
		BDirectory execDir;
};

#endif