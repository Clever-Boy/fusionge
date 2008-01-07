#pragma once

#include "SphereFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion 
{
	namespace Factories
	{
		class SphereFactory : public ObjectFactory
		{
		public:
			SphereFactory(Game* game);
			~SphereFactory(void);

			virtual Object* createInstance(lua_State* L, int luaReference, const String& name);
			virtual void destroyInstance(Object* object);
		};
	}
}
