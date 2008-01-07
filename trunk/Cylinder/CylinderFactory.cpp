#include "CylinderFactory.h"
#include "CylinderFactoryPrerequisites.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

CylinderFactory* cylinderFactory;

CylinderFactory::CylinderFactory(Game* game) : ObjectFactory("Cylinder", game) { }

CylinderFactory::~CylinderFactory(void) { }

Object* CylinderFactory::createInstance(lua_State* L, int luaReference, const String& name)
{
	int idx = lua_gettop(L);
	const char* mesh = getTableStringField(L, idx, "mesh");
	lua_Number mass = getTableNumberField(L, idx, "mass");
	Ogre::Vector3 direction = getTableVector3Field(L, idx, "direction");
	lua_Number radius = getTableNumberField(L, idx, "radius");
	lua_Number length = getTableNumberField(L, idx, "length");

	Cylinder* cylinder = new Cylinder(name, mesh, mass, radius, length, direction, mGame, mWorld, mSpace);

	cylinder->setLuaReference(luaReference);

	/* table is in the stack at index 't' */
	lua_pushstring(L, "onCollision");
	lua_gettable(L, idx);
	if (lua_isfunction(L, -1))
	{
		cylinder->getGeometry()->setUserData(OBJECT_ENABLE_COLLISION);
	}

	return cylinder;
}

void CylinderFactory::destroyInstance(Fusion::Core::Object* object)
{
	delete object;
}

extern "C" void _CylinderFactoryExport dllStartFactory(Fusion::Core::Game* game)
{
    // Create factory	
	cylinderFactory = new CylinderFactory(game);

    // Register
	game->getObjectManager()->registerObjectFactory(cylinderFactory);
}

extern "C" void _CylinderFactoryExport dllStopFactory(Fusion::Core::Game* game)
{
	game->getObjectManager()->unregisterObjectFactory(cylinderFactory);
    delete cylinderFactory;
}