#include "SphereFactory.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

SphereFactory::SphereFactory(Game* game) : ObjectFactory("Sphere", game) { }

SphereFactory::~SphereFactory(void) { }

Object* SphereFactory::createInstance(lua_State* L, int luaReference, const String& name)
{
	int idx = lua_gettop(L);
	const char* lua_mesh = getTableStringField(L, idx, "mesh");
	lua_Number lua_mass = getTableNumberField(L, idx, "mass");
	lua_Number lua_radius = getTableNumberField(L, idx, "radius");

	Sphere* sphere = new Sphere(name, String(lua_mesh), lua_mass, lua_radius, mGame, mWorld, mSpace);	

	sphere->setLuaReference(luaReference);

	/* table is in the stack at index 'idx' */
	lua_pushstring(L, "onCollision");
	lua_gettable(L, idx);
	if (lua_isfunction(L, -1))
	{
		sphere->getGeometry()->setUserData(OBJECT_ENABLE_COLLISION);
	}

	return sphere;
}

void SphereFactory::destroyInstance(Object* object)
{
	mSceneManager->destroySceneNode(object->getSceneNode()->getName());
	mSceneManager->destroyEntity(object->getEntity());

	delete object;
}