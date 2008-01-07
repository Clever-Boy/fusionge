#pragma once

#include "Prerequisites.h"

#define LUA_LIGHTHANDLE  "Fusion::Core::Light*"

namespace Fusion
{
	namespace Core
	{
		class Light
		{
		public:
			Light(const String& name, const String& type);
			virtual ~Light(void);
			
			const String& getName(void) const;

			static int _name(lua_State* L);	
			static int _type(lua_State* L);	
			static int _direction(lua_State* L);	
			static int _specularColour(lua_State* L);			
			static int _position(lua_State* L);	
			static int _visible(lua_State* L);	
			static int _tostring(lua_State *L);
		
			static int openLuaLib(lua_State *L);

		protected:
			static const luaL_Reg methods[];

			Ogre::Light* mLight;
		};
	}
}