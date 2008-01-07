#pragma once

#include "CylinderFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories 
	{
		#define LUA_CYLINDERHANDLE  "Fusion::Vehicle*"

		class Cylinder : public Object
		{
		public:
			Cylinder(const String& name,
				const String& mesh, 
				Ogre::Real mass,
				Ogre::Real radius, 
				Ogre::Real length, 
				const Ogre::Vector3& direction,
				Game* game,
				World *world, 
				OgreOde::Space* space);

			virtual ~Cylinder(void);

			virtual const String& getTypeName(void) const
			{
				static String sName("Cylinder"); return sName;
			};

			virtual Ogre::SceneNode* getSceneNode(){return mNode;}

		protected:
			Ogre::SceneNode* mNode;
		};
	}
}
