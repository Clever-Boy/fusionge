#include "StableHeaders.h"
#include "LuaHelper.h"

using namespace Fusion::Core;

const char* LuaHelper::getTableStringField(lua_State* L, int idx, const char* field)
{
	lua_pushstring(L, field);
	lua_gettable(L, idx);   // This pops "size", gets the value of "size" in the table and pushes the value
	const char* result = lua_tostring(L, -1);   // Get the returned value off the stack
	lua_pop(L, 1);

	return result;
}

lua_Number LuaHelper::getTableNumberField(lua_State* L, int idx, const char* field)
{
	lua_pushstring(L, field);
	lua_gettable(L, idx);
	lua_Number result = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

const Ogre::Vector3 LuaHelper::getTableVector3Field(lua_State* L, int idx, const char* field)
{
	Ogre::Vector3 result;

	lua_pushstring(L, field);
	lua_gettable(L, idx);

	lua_rawgeti(L, -1, 1);
	result.x = Ogre::Real(lua_tonumber(L, -1));   // Get the returned value off the stack
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 2);
	result.y = Ogre::Real(lua_tonumber(L, -1));   // Get the returned value off the stack
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 3);
	result.z = Ogre::Real(lua_tonumber(L, -1));   // Get the returned value off the stack
	lua_pop(L, 1);


	lua_pop(L, 1);

	return result;
}

const Ogre::Vector3 LuaHelper::luaL_optvector3(lua_State* L, int idx, const Ogre::Vector3& vector)
{
	Ogre::Vector3 result(vector);

	if (lua_istable(L, idx))
	{
		/* table is in the stack at index 't' */
		lua_pushnil(L);  /* first key */
		if (lua_next(L, idx) != 0) 
		{
			result.x = luaL_checknumber(L, -1);
			lua_pop(L, 1);

			if (lua_next(L, idx) != 0) 
			{
				result.y = luaL_checknumber(L, -1);
				lua_pop(L, 1);

				if (lua_next(L, idx) != 0) 
				{
					result.z = luaL_checknumber(L, -1);
					lua_pop(L, 2);
				}
			}
		}	
	}
	else if (lua_isnumber(L, idx))
	{
		result.x = luaL_checknumber(L, idx++);
		if (lua_isnumber(L, idx))
		{
			result.y = luaL_checknumber(L, idx++);
			if (lua_isnumber(L, idx))
			{
				result.z = luaL_checknumber(L, idx++);
			}
		}
	}

	return result;
}

int LuaHelper::lua_pushvector3(lua_State* L, const Ogre::Vector3& vector)
{
	lua_newtable(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, vector.x);
    lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, vector.y);
    lua_settable(L, -3);

	lua_pushstring(L, "z");
	lua_pushnumber(L, vector.z);
    lua_settable(L, -3);

	return 1;
}