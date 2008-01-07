#include "StableHeaders.h"
#include "Light.h"

using namespace Fusion::Core;

Light::Light(const String& name, const String& type)
{
	this->mLight = Game::getSingleton().getSceneManager()->createLight(name);

	if (type.compare("point") == 0)
	{
		mLight->setType(Ogre::Light::LT_POINT);
	}
	else if (type.compare("direcional") == 0)
	{
		mLight->setType(Ogre::Light::LT_DIRECTIONAL);
	}
	else if (type.compare("spotlight") == 0)
	{
		mLight->setType(Ogre::Light::LT_SPOTLIGHT);
	}

	mLight->setCastShadows(true);
}

Light::~Light(void)
{
	Game::getSingleton().getSceneManager()->destroyLight(mLight);
}

const String& Light::getName(void) const
{
	return mLight->getName();
}

#define topLight(L)	((Light **)lua_touserdata(L, 1))

static Light* toLight(lua_State *L) 
{
	Light **light = topLight(L);
	if (*light == NULL)
		luaL_error(L, "attempt to use an invalid Light");

	return *light;
}

int Light::_name(lua_State* L)
{
	Light* light = toLight(L);
	lua_pushstring(L, light->getName().c_str());
	return 1;	
}

int Light::_type(lua_State* L)
{
	Light* light = toLight(L);

	switch(light->mLight->getType())
	{
	case Ogre::Light::LT_POINT:
		lua_pushstring(L, "point");
		return 1;
	case Ogre::Light::LT_DIRECTIONAL:
		lua_pushstring(L, "directional");
		return 1;
	case Ogre::Light::LT_SPOTLIGHT:
		lua_pushstring(L, "spotlight");
		return 1;
	}

	return 0;
}

int Light::_direction(lua_State* L)
{
	Light* light = toLight(L);
	const Ogre::Vector3 direction = LuaHelper::luaL_optvector3(L, 2);

	light->mLight->setDirection(direction);
	
	return 0;
}

int Light::_specularColour(lua_State* L)
{
	Light* light = toLight(L);
	const Ogre::Vector3 colour = LuaHelper::luaL_optvector3(L, 2);

	light->mLight->setSpecularColour(colour.x, colour.y, colour.z);
	
	return 0;
}

int Light::_position(lua_State* L)
{
	Light* light = toLight(L);
	const Ogre::Vector3 position = LuaHelper::luaL_optvector3(L, 2);

	light->mLight->setPosition(position);

	return 0;
}

int Light::_visible(lua_State* L)
{
	return 0;
}

int Light::_tostring(lua_State *L) 
{
	Light* obj = toLight(L);
	lua_pushfstring(L, "Light - %s", obj->getName().c_str());
	return 1;
}

const luaL_Reg Light::methods[] = {
	{"name", &Light::_name},
	{"type", &Light::_type},
	{"direction", &Light::_direction},
	{"specularColour", &Light::_specularColour},
	{"position", &Light::_position},
	{"visible", &Light::_visible},
	{"__tostring", &Light::_tostring},
	{NULL, NULL}
};

int Light::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_LIGHTHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}