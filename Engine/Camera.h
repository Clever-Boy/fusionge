#pragma once

#include "Prerequisites.h"

#define LUA_CAMERAHANDLE  "Fusion::Core::Camera*"

namespace Fusion
{
	namespace Core
	{
		class Camera : public Ogre::FrameListener
		{
		public:
			Camera(const String& name, Ogre::SceneManager* sceneManager);
			~Camera(void);

			virtual bool frameStarted(const Ogre::FrameEvent& evt);
			virtual bool frameEnded(const Ogre::FrameEvent& evt);

			inline Ogre::Camera* getOgreCamera(void) {
				return camera;
			}

			inline void setPosition(const Ogre::Vector3& position) {
				camera->setPosition(position);
			}

			inline const Ogre::Vector3& getPosition(void) const {
				return camera->getPosition();
			}

			inline void setOrientation(const Ogre::Quaternion& orientation) {
				camera->setOrientation(orientation);
			}

			inline const Ogre::Quaternion& getOrientation(void) const {
				return camera->getOrientation();
			}

			Ogre::Real getFollowFactor(void); 
			Ogre::Real getHeight(void);
			Ogre::Real getDistance(void);
			Ogre::Real getLookAhead(void);

			void setFollowFactor(Ogre::Real value); 
			void setHeight(Ogre::Real value);
			void setDistance(Ogre::Real value);
			void setLookAhead(Ogre::Real value);

			void setTrackingObject(Object* obj);		
			Object * getTrackingObject(void);		

			static int _position(lua_State *L);
			static int _orientation(lua_State *L) ;
			static int _track(lua_State *L);
			static int _lookat(lua_State *L);

			static int openLuaLib(lua_State *L);

		protected:
			Ogre::Camera* camera;
			Object* trackingObject;

			Ogre::Real followFactor; 
			Ogre::Real height; 
			Ogre::Real distance; 
			Ogre::Real lookAhead;

			static const luaL_Reg methods[];
		};
	}
}
