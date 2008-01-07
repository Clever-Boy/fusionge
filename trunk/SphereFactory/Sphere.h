#pragma once

#include "SphereFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories
	{
		class Sphere : 	public Object
		{
		public:
			Sphere(const String& name,
				   const String& mesh, 
				   Ogre::Real mass,
				   Ogre::Real radius,
				   Game* game,
				   World *world, 
				   OgreOde::Space* space);

			virtual ~Sphere(void);

			virtual const String& getTypeName(void) const
			{
				static String sName("Sphere"); return sName;
			};
			
			virtual Ogre::SceneNode* getSceneNode(){return mNode;}

		protected:
			Ogre::SceneNode* mNode;
		};
	}
}