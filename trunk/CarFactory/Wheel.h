#pragma once

#include "VehicleFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories 
	{
		class Vehicle;

		class Wheel : public Object
		{
		public:
			static const Ogre::Real FudgeFactor;

			Wheel(Vehicle* vehicle,
				const String& name,
				const String& mesh,
				const Ogre::Vector3& position, 
				Ogre::Real mass, 
				Game* game,
				World *world, 
				OgreOde::Space* space);

			~Wheel();

			virtual const String& getTypeName(void) const
			{
				static String typeName("Wheel"); return typeName;
			};

			virtual Ogre::SceneNode* getSceneNode(){return mNode;}

			void setFactors(Ogre::Real steer, Ogre::Real power, Ogre::Real brake);
			void setContact(Ogre::Real bouncyness, Ogre::Real friction, Ogre::Real fds)
			{
				_bouncyness = bouncyness;
				_friction = friction;
				_fds = fds;
			}

			void setSteerFactor(Ogre::Real factor);
			void setSteerLimit(Ogre::Real limit);
			void setSteerForce(Ogre::Real force){_steer_force = force;}
			void setSteerSpeed(Ogre::Real speed){_steer_speed = speed;}
			void setSteer(Ogre::Real position);

			void setPowerFactor(Ogre::Real power){_power_factor = power;}
			void setBrakeFactor(Ogre::Real brake){_brake_factor = brake;}

			void setSuspension(Ogre::Real spring_constant, Ogre::Real damping_constant, Ogre::Real step_size);

			Ogre::Real getRPM();
			Ogre::Real getSteer();

			void update(Ogre::Real power_force, Ogre::Real desired_rpm, Ogre::Real brake_force);

			Ogre::Real getRadius()const{return _radius;}

			void snapOff();

			void setupTyreContact(OgreOde::Contact* contact);

		protected:
			void adjustJointParameters();

			Ogre::SceneNode*                mNode;
			OgreOde::SuspensionJoint*		mJoint;
			Ogre::Real				    	_steer_factor,_steer_limit,_steer_force,_steer_speed;
			Ogre::Real				    	_power_factor;
			Ogre::Real				    	_brake_factor;
			Ogre::Real				    	_spring,_damping,_step;
			Ogre::Real				    	_bouncyness,_friction,_fds;
			Ogre::Real				    	_radius;
			Vehicle*				        _owner;
		};
	}
}
