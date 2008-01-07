#pragma once

#include "VehicleFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories
	{
	#define LUA_VEHICLEHANDLE  "Fusion::Vehicle*"

	class Engine;
	class Wheel;

    class Vehicle : public Object
    {
	public:
        Vehicle(const String& name, 
				const String& mesh, 
				Ogre::Real mass, 
				Game* game,
				World *world, 
				OgreOde::Space* space,
				const Ogre::Vector3& offset = Ogre::Vector3::ZERO,
				OgreOde::Geometry* geometry = 0);

        Vehicle(const String& name, 
				Game* game,
                World *world, 
                OgreOde::Space* space = 0,
                OgreOde::Geometry* geometry = 0);

		~Vehicle();

		virtual const String& getTypeName(void) const
        {
			static String typeName("Vehicle"); return typeName;
		};

		virtual const String& getLuaHandle(void) const 
		{ 
			static String luaHandle(LUA_VEHICLEHANDLE); return luaHandle;
		}

		virtual void setSleep(bool value);

        Wheel* addWheel(const String& mesh, const Ogre::Vector3& position, Ogre::Real mass);
		
		virtual const Ogre::Vector3& getPosition(void);
		virtual void setPosition(const Ogre::Vector3& position);

		void setSuspension(Ogre::Real spring_constant, Ogre::Real damping_constant, Ogre::Real step_size);
		
		void setInputs(Ogre::Real steering, Ogre::Real throttle,Ogre::Real brake);
		void setInputs(Ogre::Real steering, Ogre::Real throttle_brake);
		void setInputs(Ogre::Real time, bool left, bool right, bool throttle, bool brake);

		void setSteering(Ogre::Real steering_position);

		virtual void applyAntiSwayBarForces();
		virtual void update(Ogre::Real time);

		virtual Ogre::SceneNode*	getSceneNode(){return mNode;}

		Wheel*		        findWheel(OgreOde::Geometry* geometry);
		Wheel*		        getWheel(unsigned int i){return _wheels[i];}
		size_t				getWheelCount()const{return _wheels.size();}

		OgreOde::TransformGeometry*	getTransformGeometry(){return _transform;}
		void				setTransformGeometry(OgreOde::TransformGeometry*g){_transform = g;}

        void				setTransNode(Ogre::SceneNode *s){_trans_node = s;}
		Ogre::SceneNode*	getTransNode(){return _trans_node;}		

		Engine*				getEngine(){return mEngine;}		

		OgreOde::Space*		getInternalSpace(){return iSpace;}

        Ogre::Vector3		getOffset(){return _offset;}
		void				setOffset(const Ogre::Vector3 &of)
		{
			_offset = of;
			if (mGeometry) 
				mGeometry->setPosition(of);
			_trans_node->setPosition(of);
		}

		Ogre::Real getVelocity();
		
		virtual bool handleCollision(OgreOde::Contact* contact);

		bool isAntiSwayEnabled()const {return _antisway;};
		void enableAntiSway(bool enable){_antisway = enable;};

		void  setSwayForce(Ogre::Real swayForce){_swayForce = swayForce;};
		void  setSwayForceLimit(unsigned int swayForceLimit){_swayForceLimit = swayForceLimit;}
		void  setSwayForceRate(Ogre::Real swayRate){_swayRate = swayRate;}

		Ogre::Real	  getSwayForce (Ogre::Real swayForce) const{return _swayForce;};
		unsigned int  getSwayForceLimit(unsigned int swayForceLimit) const{return _swayForceLimit;}
		Ogre::Real    getSwayForceRate(Ogre::Real swayRate) const{return _swayRate;}

		void setEngineSound(SoundSource* soundSource) 
		{ 
			mEngineSound = soundSource; 
			if (mEngineSound != NULL && mEngineSound->getState() != SoundSource::Playing)
				mEngineSound->play();
		}

		SoundSource* getEngineSound(void) { return mEngineSound; }
		
		void updateEngineSound(float level);

	protected:
		Ogre::SceneNode*                        mNode;
		Ogre::SceneNode*						_trans_node;
        OgreOde::TransformGeometry*				_transform;
		std::vector<Wheel*>	                    _wheels;
        OgreOde::SimpleSpace*					iSpace;
        Ogre::Vector3							_offset;
		Ogre::Vector3							_lastPosition;
		Engine*							        mEngine;
		SoundSource*                            mEngineSound;

		bool							_antisway;
		Ogre::Real						_swayForce;
		unsigned int					_swayForceLimit;
		Ogre::Real						_swayRate;
		Ogre::Real						_swayLastUpdate;

		bool leftPressed;
		bool rightPressed;
		bool throttlePressed;
		bool brakePressed;

	public:
		static int openLuaLib(lua_State *L);

		static int _left(lua_State *L);
		static int _right(lua_State *L);
		static int _throttle(lua_State *L);
		static int _brake(lua_State *L);

	public:
		static const luaL_Reg methods[];
	};
	}
}
