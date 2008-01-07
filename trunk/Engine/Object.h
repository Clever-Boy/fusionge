#pragma once

#include "Prerequisites.h"

#define LUA_OBJECTHANDLE  "Fusion::Core::Object*"
#define OBJECT_ENABLE_COLLISION 123456

namespace Fusion
{
	namespace Core
	{
		class Game;

		class Object : OIS::MouseListener
		{
		public:
			Object(Game* game, World* world, OgreOde::Space* space, const String& name = "");

			Object(Game* game, World* world, OgreOde::Space* space, OgreOde::Geometry* geometry, const String& name = "");

			virtual ~Object(void);

			virtual const String& getTypeName(void) const = 0;
			virtual const String& getName(void) const;

			virtual void setClassName(const String& name) 
			{
				this->mClassName = name;
			}

			virtual const String& getClassName(void) const 
			{
				return this->mClassName;
			}

			virtual const String& getLuaHandle(void) const 
			{ 
				static String luaHandle(LUA_OBJECTHANDLE); return luaHandle;
			}

			virtual OgreOde::Body* getBody(void);
			virtual void setBody(OgreOde::Body* body);

			virtual void setGeometry(OgreOde::Geometry* g){mGeometry = g;}
			virtual void setEntity(Ogre::Entity* e){mEntity = e;}

			virtual Ogre::SceneNode*    getSceneNode(void) = 0;
			virtual Ogre::Entity*	    getEntity(void){return mEntity;}
			virtual OgreOde::Geometry*  getGeometry(void){return mGeometry;}

			virtual World* getWorld(void);
			virtual OgreOde::Space* getSpace(void);

			virtual const Ogre::Vector3& getPosition(void);
			virtual const Ogre::Quaternion& getOrientation(void);
			virtual void setPosition(const Ogre::Vector3& position);

			virtual int getLuaReference(void);
			virtual void setLuaReference(int reference);

			virtual void update(Ogre::Real time) { }
			virtual bool handleCollision(OgreOde::Contact* contact);

			virtual bool isSleep(void);
			virtual void setSleep(bool value);

			virtual bool mouseMoved( const OIS::MouseEvent &arg );
			virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
			virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

		protected:		
			int                 mLuaReference;
			String              mName;
			String              mClassName;
			OgreOde::Body*      mBody;		
			Ogre::Entity*	    mEntity;
			OgreOde::Geometry*	mGeometry;
			World*              mWorld;
			OgreOde::Space*     mSpace;
			Game*               mGame;
			bool                mIsCollisionEnabled;

		public:
			static int openLuaLib(lua_State *L);
			static int openLuaLib(lua_State *L, const char* handleName, const luaL_Reg newMethods[]);

			static int _name(lua_State *L);
			static int _class(lua_State *L);
			static int _position(lua_State *L);
			static int _linearVelocity(lua_State *L) ;
			static int _angularVelocity(lua_State *L);
			static int _force(lua_State *L);
			static int _torque(lua_State *L);
			static int _addForce(lua_State *L);
			static int _addRelativeForce(lua_State *L);
			static int _addTorque(lua_State *L);
			static int _addRelativeTorque(lua_State *L);
			static int _orientation(lua_State *L);
			static int _sleep(lua_State *L);
			static int _tostring(lua_State *L);		

		protected:
			static const luaL_Reg methods[];
		};
	}
}