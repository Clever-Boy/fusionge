#pragma once

#include "Prerequisites.h"

#define LUA_GEOMETRYHANDLE  "Fusion::Core::Geometry*"

namespace Fusion
{
	namespace Core
	{		
		class Geometry
		{
		public:
			Geometry(const String& name, OgreOde::Geometry* geometry, Ogre::Node* node);
			virtual ~Geometry(void);

			void setPosition(const Ogre::Vector3& position);
			const Ogre::Vector3& getPosition(void) const;

			void setOrientation(const Ogre::Quaternion& orientation);
			const Ogre::Quaternion& getOrientation(void) const;

			static int _name(lua_State *L);
			static int _position(lua_State *L);
			static int _orientation(lua_State *L);
			static int _tostring(lua_State *L);

			static int openLuaLib(lua_State *L);

		protected:
			static const luaL_Reg methods[];

			String             mName;
			OgreOde::Geometry* mGeometry;
			Ogre::Node*        mNode;
		};
	}
}