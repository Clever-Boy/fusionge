#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		#define LUA_WORLDHANDLE  "Fusion::Core::World*"

		class World : public OgreOde::World
		{
		public:
			World(Ogre::SceneManager* manager);
			virtual ~World(void);

			Geometry* createGeometry(const String& name, const String& mesh);

			static int _create(lua_State* L);
			static int _destroy(lua_State* L);
			static int _geometry(lua_State* L);		
			static int _skybox(lua_State* L);
			static int _skydome(lua_State* L);
			static int _ambientLight(lua_State* L);	
			static int _light(lua_State* L);
			static int _shadow(lua_State* L);		
			static int _sound(lua_State* L);
			static int _gravity(lua_State* L);
			static int _cfm(lua_State* L);
			static int _erp(lua_State* L);		
		
			static int openLuaLib(lua_State *L);

		protected:
			static const luaL_Reg methods[];	
		};
	}
}
