#include "StableHeaders.h"

#include "Camera.h"

using namespace Fusion::Core;

Camera::Camera(const String& name, Ogre::SceneManager* sceneManager)
{
	this->camera = sceneManager->createCamera(name);
	this->camera->setPosition(13,4.5,0);
	this->camera->lookAt(0,0.5,0);
	this->camera->setNearClipDistance(0.5);
	this->trackingObject = NULL;
	this->followFactor = 0.6; 
	this->height = 2.0; 
	this->distance = 10.0; 
	this->lookAhead = 8.0;
}

Camera::~Camera(void) 
{ 
	Ogre::SceneManager* sceneManager = this->camera->getSceneManager();
	sceneManager->destroyCamera(this->camera);
}

bool Camera::frameStarted(const Ogre::FrameEvent& evt)
{
	if (trackingObject != NULL)
	{
		const Ogre::Quaternion q = trackingObject->getOrientation(); 
		Ogre::Vector3 toCam = trackingObject->getPosition(); 

		toCam.y += this->height; 
		toCam.z -= this->distance * q.zAxis().z; 
		toCam.x -= this->distance * q.zAxis().x; 
	      
		camera->move( (toCam - camera->getPosition()) * this->followFactor ); 
		camera->lookAt(trackingObject->getPosition() + ((trackingObject->getOrientation() * Ogre::Vector3::UNIT_Z) * this->lookAhead));		
	}

	return true;
}

bool Camera::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}

Ogre::Real Camera::getFollowFactor(void)
{
	return this->followFactor;
}

Ogre::Real Camera::getHeight(void)
{
	return this->height;
}

Ogre::Real Camera::getDistance(void)
{
	return this->distance;
}

Ogre::Real Camera::getLookAhead(void)
{
	return this->lookAhead;
}

void Camera::setFollowFactor(Ogre::Real value)
{
	this->followFactor = value;
}

void Camera::setHeight(Ogre::Real value)
{
	this->height = value;
}

void Camera::setDistance(Ogre::Real value)
{
	this->distance = value;
}

void Camera::setLookAhead(Ogre::Real value)
{
	this->lookAhead = value;
}

void Camera::setTrackingObject(Object* obj)
{
	this->trackingObject = obj;	
}

Object* Camera::getTrackingObject(void)
{
	return this->trackingObject;
}

#define topCamera(L)	((Camera **)luaL_checkudata(L, 1, LUA_CAMERAHANDLE))

const luaL_Reg Camera::methods[] = {
	{"position", &Camera::_position}, 
	{"orientation", &Camera::_orientation},
	{"track", &Camera::_track},
	{"lookat", &Camera::_lookat},
	{NULL, NULL}
};

static Camera* toCamera(lua_State *L) 
{
	Camera **obj = topCamera(L);
	if (*obj == NULL)
		luaL_error(L, "attempt to use an invalid Camera object");

	return *obj;
}

int Camera::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_CAMERAHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}

int Camera::_position(lua_State *L)
{
	Camera* camera = toCamera(L);

	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Vector3 value = camera->getPosition();
		return LuaHelper::lua_pushvector3(L, value);
	} 
	else 
	{
		Ogre::Vector3 value = camera->getPosition();
		value = LuaHelper::luaL_optvector3(L, 2, value);
		camera->setPosition(value);
		return 0;
	}
}

int Camera::_orientation(lua_State *L) 
{
	Camera* camera = toCamera(L);

	if (lua_isnoneornil(L, 2))
	{
		const Ogre::Quaternion orientation = camera->getOrientation();
		lua_pushnumber(L, orientation.x);
		lua_pushnumber(L, orientation.y);
		lua_pushnumber(L, orientation.z);
		lua_pushnumber(L, orientation.w);
		return 4;
	} 
	else 
	{
		Ogre::Quaternion orientation(camera->getOrientation());
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
		
		camera->setOrientation(orientation);
		return 0;
	}
}

int Camera::_track(lua_State *L)
{
	Camera* camera = toCamera(L);
	Object** obj = (Object **)lua_touserdata(L, 2);
	if (*obj == NULL)
		luaL_error(L, "attempt to use an invalid Object");

	camera->setTrackingObject(*obj);

	return 0;
}

int Camera::_lookat(lua_State *L)
{
	Camera* camera = toCamera(L);
	Ogre::Vector3 target = LuaHelper::luaL_optvector3(L, 2, Ogre::Vector3::ZERO);

	camera->getOgreCamera()->lookAt(target);

	return 0;
}