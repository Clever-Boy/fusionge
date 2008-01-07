#include "StableHeaders.h"
#include "Object.h"
#include "LuaHelper.h"
#include "Game.h"

using namespace Fusion::Core;

Object::Object(Game* game, World* world, OgreOde::Space* space, const String& name) 
: mGame(game), mWorld(world), mSpace(space), mName(name), mClassName(""), mGeometry(NULL), mEntity(NULL), mIsCollisionEnabled(false)
{ 
	mBody = new OgreOde::Body(mWorld, name);
}

Object::Object(Game* game, World* world, OgreOde::Space* space, OgreOde::Geometry* geometry, const String& name) 
: mGame(game), mWorld(world), mSpace(space), mName(name), mClassName(""), mGeometry(geometry), mEntity(NULL), mIsCollisionEnabled(false) 
{ 
	mBody = new OgreOde::Body(mWorld, name);
}

Object::~Object(void) 
{
	if (mGeometry)
	{
		delete mGeometry;
	}

	if (mEntity)
	{
		mWorld->getSceneManager()->destroyEntity(mEntity);
	}

	delete mBody;
}

OgreOde::Body* Object::getBody(void)
{
	return this->mBody;
}

void Object::setBody(OgreOde::Body* body)
{
	this->mBody = body;
}

World* Object::getWorld(void)
{
	return this->mWorld;
}

OgreOde::Space* Object::getSpace(void)
{
	return this->mSpace;
}

const String& Object::getName(void) const
{
	return this->mName;
}

int Object::getLuaReference(void)
{
	return this->mLuaReference;
}

void Object::setLuaReference(int reference)
{
	this->mLuaReference = reference;
}

const Ogre::Vector3& Object::getPosition()
{
	return mBody->getPosition();
}

const Ogre::Quaternion& Object::getOrientation()
{
	return mBody->getOrientation();
}

void Object::setPosition(const Ogre::Vector3& position)
{
	mBody->setPosition(position);
}

bool Object::isSleep(void)
{
	return mBody->isAwake();
}

void Object::setSleep(bool value)
{
	if (!value) 
	{
		mBody->wake();
	}
	else 
	{
		mBody->sleep();
	}
}

bool Object::handleCollision(OgreOde::Contact* contact)
{
	bool result = false;

	if (contact->getFirstGeometry()->getUserObject() == this 
		&& contact->getSecondGeometry()->getUserObject() != NULL) 
	{		
		result = mGame->getScriptManager()->fireOnCollisionEvent(
						(Object*)contact->getFirstGeometry()->getUserObject(), 
						(Object*)contact->getSecondGeometry()->getUserObject()
					);
	} 
	else if (contact->getSecondGeometry()->getUserObject() == this 
			&& contact->getFirstGeometry()->getUserObject() != NULL) 
	{		
		result = mGame->getScriptManager()->fireOnCollisionEvent(
						(Object*)contact->getSecondGeometry()->getUserObject(), 
						(Object*)contact->getFirstGeometry()->getUserObject()
					);
	}

	return result;
}

bool Object::mouseMoved( const OIS::MouseEvent &arg )
{	
	return false;
}

bool Object::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}
	
bool Object::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}

#define topObject(L)	((Object **)lua_touserdata(L, 1))

static Object* toObject(lua_State *L) 
{
	Object **obj = topObject(L);
	if (*obj == NULL)
		luaL_error(L, "attempt to use an invalid Object");

	return *obj;
}

static OgreOde::Body* toBody(lua_State *L) 
{
	Object *obj = toObject(L);
	if (obj->getBody() == NULL)
		luaL_error(L, "attempt to use an invalid Object");

	return obj->getBody();
}

int Object::_name(lua_State *L) 
{
	Object* obj = toObject(L);
	lua_pushstring(L, obj->getName().c_str());
	return 1;
}

int Object::_class(lua_State *L) 
{
	Object* obj = toObject(L);
	lua_pushstring(L, obj->getClassName().c_str());
	return 1;
}

#pragma region Generic Vector3 Getter/Setter
typedef void (OgreOde::Body::*setVector3Function)(const Ogre::Vector3& value);
typedef const Ogre::Vector3& (OgreOde::Body::*getVector3Function)(void);

static int object_genericGetSetVector3(lua_State *L, getVector3Function getter, setVector3Function setter) 
{
	OgreOde::Body* body = toBody(L);

	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Vector3 value = (body->*getter)();
		return LuaHelper::lua_pushvector3(L, value);
	} 
	else 
	{
		Ogre::Vector3 value = ((body->*getter)());		
		value = LuaHelper::luaL_optvector3(L, 2, value);
		(body->*setter)(value);
		return 0;
	}
}

static int object_genericAddVector3(lua_State *L, setVector3Function addfunc) 
{
	OgreOde::Body* body = toBody(L);
	
	Ogre::Vector3 value = LuaHelper::luaL_optvector3(L, 2);	

	(body->*addfunc)(value);

	return 0;
}
#pragma endregion

int Object::_position(lua_State *L) 
{	
	OgreOde::Body* body = toBody(L);

	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Vector3 value = body->getPosition();
		return LuaHelper::lua_pushvector3(L, value);
	} 
	else 
	{
		Ogre::Vector3 value = body->getPosition();	
		value = LuaHelper::luaL_optvector3(L, 2, value);
		body->setPosition(value);
		return 0;
	}
}

int Object::_linearVelocity(lua_State *L) 
{	
	return object_genericGetSetVector3(L, &OgreOde::Body::getLinearVelocity, &OgreOde::Body::setLinearVelocity);
}

int Object::_angularVelocity(lua_State *L) 
{	
	return object_genericGetSetVector3(L, &OgreOde::Body::getAngularVelocity, &OgreOde::Body::setAngularVelocity);
}

int Object::_force(lua_State *L) 
{	
	return object_genericGetSetVector3(L, &OgreOde::Body::getForce, &OgreOde::Body::setForce);
}

int Object::_torque(lua_State *L) 
{	
	return object_genericGetSetVector3(L, &OgreOde::Body::getTorque, &OgreOde::Body::setTorque);
}

int Object::_addForce(lua_State *L)
{
	return object_genericAddVector3(L, &OgreOde::Body::addForce);
}

int Object::_addRelativeForce(lua_State *L)
{
	return object_genericAddVector3(L, &OgreOde::Body::addRelativeForce);
}

int Object::_addTorque(lua_State *L)
{
	return object_genericAddVector3(L, &OgreOde::Body::addTorque);
}

int Object::_addRelativeTorque(lua_State *L)
{
	return object_genericAddVector3(L, &OgreOde::Body::addRelativeTorque);
}

int Object::_orientation(lua_State *L) 
{	
	OgreOde::Body* body = toBody(L);
	
	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Quaternion orientation = body->getOrientation();
		lua_pushnumber(L, orientation.x);
		lua_pushnumber(L, orientation.y);
		lua_pushnumber(L, orientation.z);
		lua_pushnumber(L, orientation.w);
		return 4;
	} 
	else 
	{
		Ogre::Quaternion orientation(body->getOrientation());
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
		
		body->setOrientation(orientation);
		return 0;
	}
}

int Object::_sleep(lua_State *L) 
{
	Object* obj = toObject(L);

	if (lua_isboolean(L, 2))
	{
		bool value = lua_toboolean(L, 2) != 0;
		obj->setSleep(value);
	}
		
	lua_pushboolean(L, obj->isSleep()? 1 : 0);

	return 1;
}

int Object::_tostring(lua_State *L) 
{
	Object* obj = toObject(L);
	lua_pushfstring(L, "%s - %s", obj->getTypeName().c_str(), obj->getName().c_str());
	return 1;
}

const luaL_Reg Object::methods[] = {
	{"name", &Object::_name},
	{"class", &Object::_class},
	{"position", &Object::_position}, 
	{"orientation", &Object::_orientation},
	{"linearVelocity", &Object::_linearVelocity},
	{"angularVelocity", &Object::_angularVelocity},
	{"sleep", &Object::_sleep},
	{"force", &Object::_force},
	{"torque", &Object::_torque},
	{"addForce", &Object::_addForce},
	{"addTorque", &Object::_addTorque},
	{"addRelativeForce", &Object::_addRelativeForce},
	{"addRelativeTorque", &Object::_addRelativeTorque},
	{"__tostring", &Object::_tostring},	
	{NULL, NULL}
};

int Object::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_OBJECTHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}

int Object::openLuaLib(lua_State *L, const char* handleName, const luaL_Reg newMethods[])
{
	HashMap<Ogre::String, const luaL_Reg*> map;

	for (const luaL_Reg *l = methods; l->name; l++) 
	{
		map[l->name] = l;
	}

	for (const luaL_Reg *l = newMethods; l->name; l++) 
	{
		map[l->name] = l;
	}

	luaL_Reg* temp = (luaL_Reg*) malloc(sizeof(luaL_Reg) * (map.size() + 1));
	memset(temp, 0, sizeof(luaL_Reg) * (map.size() + 1));

	int i = 0;
	HashMap<Ogre::String, const luaL_Reg*>::iterator iter = map.begin();
	while (iter != map.end())
	{
		memcpy(&(temp[i++]), iter->second, sizeof(luaL_Reg));
		iter++;
	}

	luaL_newmetatable(L, handleName);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, temp);  /* object methods */	

	free(temp);

	return 1;
}