#pragma once

#include "BoxFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories
	{
		class Box : public Object
		{
		public:
			Box(const String& name,
				const String& mesh, 
				Ogre::Real mass,
				const Ogre::Vector3& size,
				Game* game,
				World *world, 
				OgreOde::Space* space);

			virtual ~Box(void);

			virtual const String& getTypeName(void) const
			{
				static String sName("Box"); return sName;
			};

			virtual Ogre::SceneNode* getSceneNode(){return mNode;}

		protected:
			Ogre::SceneNode* mNode;

		};
	}
}