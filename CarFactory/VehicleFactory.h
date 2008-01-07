#pragma once

#include "VehicleFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories 
	{
		class VehicleFactory : public ObjectFactory
		{
		public:
			VehicleFactory(Game* game);
			~VehicleFactory(void);

			virtual Object* createInstance(lua_State* L, int luaReference, const String& name);
			virtual void destroyInstance(Object* object);
		};
	}
}