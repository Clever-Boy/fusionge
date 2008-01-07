#pragma once

#include "CylinderFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories 
	{
		class CylinderFactory :	public ObjectFactory
		{
		public:
			CylinderFactory(Game* game);
			~CylinderFactory(void);

			virtual Object* createInstance(lua_State* L, int luaReference, const String& name);
			virtual void destroyInstance(Object* object);
		};
	}
}