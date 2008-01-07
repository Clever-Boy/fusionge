#pragma once

#include "Prerequisites.h"
#include "LuaHelper.h"

namespace Fusion
{
	namespace Core
	{
		class Game;

		class ObjectFactory : public LuaHelper
		{
		public:
			ObjectFactory(const String& _name, Game* game);
			virtual ~ObjectFactory(void);

			const String& getName(void);

			virtual Object* createInstance(lua_State* L, int luaReference, const String& name) = 0;
			virtual void destroyInstance(Object* object) = 0;

		protected:
			String name;
			Game* mGame;
			World* mWorld;
			OgreOde::Space* mSpace;
			Ogre::SceneManager* mSceneManager;
		};
	}
}
