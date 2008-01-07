#include "StableHeaders.h"
#include "World.h"

using namespace Fusion::Core;

World::World(Ogre::SceneManager* manager) : OgreOde::World(manager)
{
}

World::~World(void)
{
}

Geometry* World::createGeometry(const String& name, const String& mesh)
{	
	Ogre::Entity *entity = this->getSceneManager()->createEntity(name, mesh);	
	Ogre::SceneNode *node = this->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
	node->attachObject(entity);

	OgreOde::EntityInformer ei(entity);
	OgreOde::TriangleMeshGeometry* geom = ei.createStaticTriangleMesh(this, this->getDefaultSpace());
	geom->setDebug(false);
    entity->setUserObject(geom);
	
	return new Geometry(name, geom, node);
}

#define topWorld(L)	((World **)lua_touserdata(L, 1))

int World::_create(lua_State* L)
{
	World* world = *topWorld(L);
	const char* className = luaL_checkstring(L, 2);
	const char* name = luaL_checkstring(L, 3);

	Object* obj = ObjectManager::getSingleton().createInstance(L, className, name);

	Object **po = (Object **)lua_newuserdata(L, sizeof(Object *));
	*po = obj;
	luaL_getmetatable(L, obj->getLuaHandle().c_str());
	lua_setmetatable(L, -2);

	return 1;
}

int World::_destroy(lua_State* L)
{
	World* world = *topWorld(L);
	
	const char* name;

	if (lua_isstring(L, 2))
	{
		name = luaL_checkstring(L, 2);
	}
	else
	{
		Object* obj = *((Object **)lua_touserdata(L, 2));
		name = obj->getName().c_str();
	}

	ObjectManager::getSingleton().destroyInstance(name);

	return 0;
}

int World::_geometry(lua_State* L)
{
	World* world = *topWorld(L);
	const char* name = luaL_checkstring(L, 2);
	const char* mesh = luaL_checkstring(L, 3);
	const Ogre::Vector3 position = LuaHelper::luaL_optvector3(L, 4, Ogre::Vector3::ZERO);

	Geometry* geometry = world->createGeometry(name, mesh);
	geometry->setPosition(position);

	Geometry **po = (Geometry **)lua_newuserdata(L, sizeof(Geometry *));
	*po = geometry;
	luaL_getmetatable(L, LUA_GEOMETRYHANDLE);
	lua_setmetatable(L, -2);

	return 1;
}

int World::_skybox(lua_State* L)
{
	World* world = *topWorld(L);
	const char* name = luaL_checkstring(L, 2);
	lua_Number distance = luaL_optnumber(L, 3, 5000);

	world->getSceneManager()->setSkyBox(true, name, distance, true);

	return 0;
}

int World::_skydome(lua_State* L)
{
	World* world = *topWorld(L);
	const char* name = luaL_checkstring(L, 2);
	lua_Number curvature = luaL_optnumber(L, 3, 10);
	lua_Number tiling = luaL_optnumber(L, 4, 8);
	lua_Number distance = luaL_optnumber(L, 5, 4000);

	world->getSceneManager()->setSkyDome(true, name, curvature, tiling, distance);

	return 0;
}

int World::_ambientLight(lua_State* L)
{
	World* world = *topWorld(L);
	lua_Number r = luaL_checknumber(L, 2);
	lua_Number g = luaL_checknumber(L, 3);
	lua_Number b = luaL_checknumber(L, 4);

	world->getSceneManager()->setAmbientLight(Ogre::ColourValue(r, g, b));

	return 0;
}

int World::_light(lua_State* L)
{
	World* world = *topWorld(L);

	const char* name = luaL_checkstring(L, 2);
	const char* type = luaL_checkstring(L, 3);

	Light* light = new Light(name, type);

	Light **plight = (Light **)lua_newuserdata(L, sizeof(Light *));
	*plight = light;
	luaL_getmetatable(L, LUA_LIGHTHANDLE);
	lua_setmetatable(L, -2);

	return 1;
}

int World::_shadow(lua_State* L)
{
	World* world = *topWorld(L);

	static const Ogre::ShadowTechnique shadows[] = {
		Ogre::SHADOWTYPE_NONE, 
		Ogre::SHADOWTYPE_STENCIL_ADDITIVE, 
		Ogre::SHADOWTYPE_STENCIL_MODULATIVE,
		Ogre::SHADOWTYPE_TEXTURE_ADDITIVE,
		Ogre::SHADOWTYPE_TEXTURE_MODULATIVE,
		Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED,
		Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED
	};

	static const char *const names[] = {
		"none", 
		"stencil_additive", 
		"stencil_modulative",
		"texture_additive",
		"texture_modulative",
		"texture_additive_integrated",
		"texture_modulative_integrated",
		NULL
	};

	int op = luaL_checkoption(L, 2, "none", names);

	world->getSceneManager()->setShadowTechnique(shadows[op]);

	return 0;
}

int World::_sound(lua_State* L)
{
	World* world = *topWorld(L);
	const char* name = luaL_checkstring(L, 2);
	const char* bufferName = luaL_checkstring(L, 3);
	
	SoundBufferPtr soundPtr = Game::getSingleton().getSoundManager()->getByName(bufferName);
	if (!soundPtr.isNull())
	{
		SoundSource* soundSource = Game::getSingleton().getSoundManager()->createSoundSource(name, soundPtr.get());
		SoundSource** psound = (SoundSource**)lua_newuserdata(L, sizeof(SoundSource*));
		*psound = soundSource;
		luaL_getmetatable(L, LUA_SOUNDSOURCEHANDLE);
		lua_setmetatable(L, -2);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

int World::_gravity(lua_State* L)
{
	World* world = *topWorld(L);
	lua_Number x = luaL_checknumber(L, 2);
	lua_Number y = luaL_checknumber(L, 3);
	lua_Number z = luaL_checknumber(L, 4);
	world->setGravity(Ogre::Vector3(x, y, z));
	return 0;
}

int World::_cfm(lua_State* L)
{
	World* world = *topWorld(L);

	if (lua_isboolean(L, 2))
	{
		lua_Number cfm = luaL_checknumber(L, 2);
		world->setCFM(cfm);
	}
		
	lua_pushnumber(L, world->getCFM());

	return 1;
}

int World::_erp(lua_State* L)
{
	World* world = *topWorld(L);

	if (lua_isboolean(L, 2))
	{
		lua_Number erp = luaL_checknumber(L, 2);
		world->setERP(erp);
	}
		
	lua_pushnumber(L, world->getERP());

	return 1;
}

const luaL_Reg World::methods[] = {
	{"create", &World::_create},
	{"destroy", &World::_destroy},
	{"geometry", &World::_geometry},
	{"skybox", &World::_skybox},
	{"skydome", &World::_skydome},
	{"ambientLight", &World::_ambientLight},
	{"light", World::_light},
	{"shadow", World::_shadow},
	{"sound", World::_sound},
	{"gravity", World::_gravity},
	{"cfm", World::_cfm},
	{"erp", World::_erp},	
	{NULL, NULL}
};

int World::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_WORLDHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}