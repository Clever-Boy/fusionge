#pragma once

#include "BoxFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion 
{
	namespace Factories
	{
		class BoxFactory : public ObjectFactory
		{
		public:
			BoxFactory(Game* game);
			~BoxFactory(void);

			virtual Object* createInstance(lua_State* L, int luaReference, const String& name);
			virtual void destroyInstance(Object* object);
		};
	}
}