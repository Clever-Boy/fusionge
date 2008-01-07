#include "StableHeaders.h"
#include "ScriptManager.h"

extern "C" {
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

using namespace Fusion::Core;

#pragma region Ogre Singleton
template<> ScriptManager* Ogre::Singleton<ScriptManager>::ms_Singleton = 0;

ScriptManager* ScriptManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

ScriptManager& ScriptManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );
}
#pragma endregion

#pragma region Lua trace functions
static void l_message (const char *msg) {
	Ogre::LogManager::getSingleton().logMessage(msg);
}

static int report (lua_State *L, int status) {
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    l_message(msg);
    lua_pop(L, 1);
  }
  return status;
}

static void lstop (lua_State *L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(L, NULL, 0, 0);
  luaL_error(L, "interrupted!");
}

static int traceback (lua_State *L) {
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);  /* pass error message */
  lua_pushinteger(L, 2);  /* skip this function and traceback */
  lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}
#pragma endregion

ScriptManager::ScriptManager()
{
	Ogre::LogManager::getSingleton().logMessage("************************************");
	Ogre::LogManager::getSingleton().logMessage("Using "LUA_RELEASE);
	Ogre::LogManager::getSingleton().logMessage("************************************");

	registerLuaLib(luaopen_base);
	registerLuaLib(luaopen_package);
	registerLuaLib(luaopen_table);
	registerLuaLib(luaopen_io);
	registerLuaLib(luaopen_os);
	registerLuaLib(luaopen_string);
	registerLuaLib(luaopen_math);
	registerLuaLib(luaopen_debug);

	registerLuaLib(&Game::openLuaLib);
	registerLuaLib(&Camera::openLuaLib);
	registerLuaLib(&Object::openLuaLib);
	registerLuaLib(&Geometry::openLuaLib);
	registerLuaLib(&Debug::openLuaLib);
	registerLuaLib(&World::openLuaLib);
	registerLuaLib(&Light::openLuaLib);
	registerLuaLib(&SoundSource::openLuaLib);

	this->L = lua_open();
}

ScriptManager::~ScriptManager(void)
{
	lua_close(this->L);
	this->L = NULL;
}

bool ScriptManager::execute(const String& file)
{
	lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
	
	std::vector<lua_CFunction>::iterator itr = luaLibs.begin();
	while (itr != luaLibs.end())
	{		
		lua_pushcfunction(L, *itr);
		lua_call(L, 0, 0);		
		itr++;
	}

	lua_gc(L, LUA_GCRESTART, 0);

	int status = luaL_loadfile(L, file.c_str());
	if (status == 0) 
	{
		int base = lua_gettop(L);  /* function index */
		lua_pushcfunction(L, traceback);  /* push traceback function */
		lua_insert(L, base);  /* put it under chunk and args */
		status = lua_pcall(L, 0, 0, base);
		lua_remove(L, base);  /* remove traceback function */
		/* force a complete garbage collection in case of errors */
		if (status != 0) 
			lua_gc(L, LUA_GCCOLLECT, 0);
	}

	report(L, status);

	return status == 0;
}

void ScriptManager::registerLuaLib(lua_CFunction func)
{
	luaLibs.push_back(func);
}

void ScriptManager::registerLuaClass(const String& name, int luaRef)
{
	luaClasses[name] = luaRef;
}

int ScriptManager::getLuaClass(const String& name)
{
	return luaClasses[name];
}

void ScriptManager::setKeyboardActionMap(int luaRef)
{
	this->keyboardActionMap = luaRef;
}

void ScriptManager::setMouseActionMap(int luaRef)
{
	this->mouseActionMap = luaRef;
}

bool ScriptManager::fireOnKeyEvent(const OIS::KeyEvent &arg, bool pressed)
{
	bool consumed = false;

	if (L != NULL && this->keyboardActionMap > 0)
	{
		lua_getref(L, this->keyboardActionMap); // põe a tabela na pilha
		int t = lua_gettop(L);

		OIS::Keyboard* keyboard = (OIS::Keyboard*)arg.device;
		const std::string& key = keyboard->getAsString(arg.key);

		/* table is in the stack at index 't' */
		lua_pushstring(L, key.c_str());
		lua_gettable(L, t);
		if (lua_isfunction(L, -1))
		{			
			lua_pushstring(L, key.c_str());
			lua_pushboolean(L, pressed);
			lua_call(L, 2, 0);
			consumed = true;
		}

		lua_settop(L, t - 1);
	}

	return consumed;
}

bool ScriptManager::fireOnCollisionEvent(Object* objA, Object* objB)
{
	bool consumed = false;

	if (L != NULL)
	{
		lua_getref(L, objA->getLuaReference()); // põe a tabela na pilha
		int t = lua_gettop(L);

		/* table is in the stack at index 't' */
		lua_pushstring(L, "onCollision");
		lua_gettable(L, t);
		if (lua_isfunction(L, -1))
		{		
			Object **pobjA = (Object **)lua_newuserdata(L, sizeof(Object *));
			*pobjA = objA;
			luaL_getmetatable(L, objA->getLuaHandle().c_str());
			lua_setmetatable(L, -2);

			Object **pobjB = (Object **)lua_newuserdata(L, sizeof(Object *));
			*pobjB = objB;
			luaL_getmetatable(L, objB->getLuaHandle().c_str());
			lua_setmetatable(L, -2);
			if( lua_pcall(L, 2, 1, 0) != 0 )
			{
				luaL_error(L, "error running function 'onCollision': %s", lua_tostring(L, -1));
				return false;
			}
			
			consumed = lua_toboolean(L, -1) != 0;
		}

		lua_settop(L, t - 1);
	}

	return consumed;
}

bool ScriptManager::keyPressed( const OIS::KeyEvent &arg )
{
	return this->fireOnKeyEvent(arg, true);
}

bool ScriptManager::keyReleased( const OIS::KeyEvent &arg )
{
	return this->fireOnKeyEvent(arg, false);
}