#include "StableHeaders.h"
#include "Geometry.h"
#include "LuaHelper.h"

using namespace Fusion::Core;

Geometry::Geometry(const String& name, OgreOde::Geometry* geometry, Ogre::Node* node) 
: mName(name), mGeometry(geometry), mNode(node)
{
}

Geometry::~Geometry(void)
{
}

void Geometry::setPosition(const Ogre::Vector3& position)
{
	this->mGeometry->setPosition(position);
	if (this->mNode != NULL) 
	{
		this->mNode->setPosition(position);
	}
}

const Ogre::Vector3& Geometry::getPosition(void) const
{
	return this->mGeometry->getPosition();
}

void Geometry::setOrientation(const Ogre::Quaternion& orientation)
{
	this->mGeometry->setOrientation(orientation);
	if (this->mNode != NULL) 
	{
		this->mNode->setOrientation(orientation);
	}
}

const Ogre::Quaternion& Geometry::getOrientation(void) const
{
	return this->mGeometry->getOrientation();
}

#define topGeometry(L)	((Geometry **)lua_touserdata(L, 1))

static Geometry* toGeometry(lua_State *L) 
{
	Geometry **obj = topGeometry(L);
	if (*obj == NULL)
		luaL_error(L, "attempt to use an invalid Geometry");

	return *obj;
}

int Geometry::_name(lua_State *L)
{
	Geometry* geometry = toGeometry(L);
	lua_pushstring(L, geometry->mName.c_str());
	return 1;
}

int Geometry::_position(lua_State *L)
{
	Geometry* geometry = toGeometry(L);

	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Vector3 value = geometry->getPosition();
		return LuaHelper::lua_pushvector3(L, value);
	} 
	else 
	{
		Ogre::Vector3 value = geometry->getPosition();
		value = LuaHelper::luaL_optvector3(L, 2, value);
		geometry->setPosition(value);
		return 0;
	}
}

int Geometry::_orientation(lua_State *L) 
{	
	Geometry* geometry = toGeometry(L);
	
	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Quaternion orientation = geometry->getOrientation();
		lua_pushnumber(L, orientation.x);
		lua_pushnumber(L, orientation.y);
		lua_pushnumber(L, orientation.z);
		lua_pushnumber(L, orientation.w);
		return 4;
	} 
	else 
	{
		Ogre::Quaternion orientation(geometry->getOrientation());
		if (lua_isnumber(L, 2))
		{
			orientation.x = Ogre::Real(luaL_checknumber(L, 2));
			if (lua_isnumber(L, 3))
			{
				orientation.y = Ogre::Real(luaL_checknumber(L, 3));
				if (lua_isnumber(L, 4))
				{
					orientation.z = Ogre::Real(luaL_checknumber(L, 4));
					if (lua_isnumber(L, 5))
					{
						orientation.w = Ogre::Real(luaL_checknumber(L, 5));
					}
				}
			}
		}
		
		geometry->setOrientation(orientation);
		return 0;
	}
}

int Geometry::_tostring(lua_State *L) 
{
	Geometry* obj = toGeometry(L);
	Ogre::Vector3 position = obj->getPosition();
	lua_pushfstring(L, "Geometry - %s (%f, %f, %f)", obj->mName.c_str(), position.x, position.y, position.z);
	return 1;
}

const luaL_Reg Geometry::methods[] = {
	{"name", &Geometry::_name},
	{"position", &Geometry::_position}, 
	{"orientation", &Geometry::_orientation},
	{"__tostring", &Geometry::_tostring},
	{NULL, NULL}
};

int Geometry::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_GEOMETRYHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}