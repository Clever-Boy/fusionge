
#include "BoxFactory.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

BoxFactory::BoxFactory(Game* game) : ObjectFactory("Box", game) { }

BoxFactory::~BoxFactory(void) { }

Object* BoxFactory::createInstance(lua_State* L, int luaReference, const String& name)
{
	int idx = lua_gettop(L);
	const char* lua_mesh = getTableStringField(L, idx, "mesh");
	lua_Number lua_mass = getTableNumberField(L, idx, "mass");
	Ogre::Vector3 lua_size = getTableVector3Field(L, idx, "size");

	Box* box = new Box(name, Ogre::String(lua_mesh), lua_mass, lua_size, mGame, mWorld, mSpace);

	box->setLuaReference(luaReference);

	/* table is in the stack at index 'idx' */
	lua_pushstring(L, "onCollision");
	lua_gettable(L, idx);
	if (lua_isfunction(L, -1))
	{
		box->getGeometry()->setUserData(OBJECT_ENABLE_COLLISION);
	}

	return box;
}

void BoxFactory::destroyInstance(Object* object)
{
	delete object;
}
