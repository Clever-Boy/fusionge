#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class LuaHelper
		{
		public:
			static const char*          getTableStringField(lua_State* L, int idx, const char* field);
			static lua_Number           getTableNumberField(lua_State* L, int idx, const char* field);
			static const Ogre::Vector3  getTableVector3Field(lua_State* L, int idx, const char* field);

			static const Ogre::Vector3  luaL_optvector3(lua_State* L, int idx, const Ogre::Vector3& vector = Ogre::Vector3::ZERO);
			
			static int lua_pushvector3(lua_State* L, const Ogre::Vector3& vector);
		};
	}
}
