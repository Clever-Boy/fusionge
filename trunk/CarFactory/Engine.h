#pragma once

#include "VehicleFactoryPrerequisites.h"

using namespace Fusion::Core;

namespace Fusion
{
	namespace Factories
	{
		class Engine
		{
		public:
			Engine(Vehicle* vehicle);
			~Engine();

			void setInputs(Ogre::Real throttle_position, Ogre::Real brake_position);
			void setInputs(Ogre::Real throttle_brake);
			void setInputs(Ogre::Real time, bool throttle, bool brake);

			Ogre::Real getBrakeForce(void);
			Ogre::Real getPowerAtRPM(Ogre::Real rpm);
			Ogre::Real getDesiredRPM(void);

			void update(Ogre::Real time);

			// Manual gearbox
			void changeUp(void);
			void changeDown(void);
			void setGear(char code);
			void setGear(unsigned int i);

			// Information functions
			Ogre::Real getRevFactor(void);
			char getGearCode(void);

			// Engine parameters
			void setRevLimit(Ogre::Real rpm);
			void setTorque(Ogre::Real* torque,unsigned int samples);
			void setTorque(Ogre::Real constant);
			void setTorque(Ogre::Real minimum, Ogre::Real maximum);

			// Braking parameters
			void setBrakeForce(Ogre::Real force);

			// Drivetrain parameters
			void addGear(Ogre::Real ratio, char code);
			void setFinalDrive(Ogre::Real ratio);
			void setAutoShiftRevs(Ogre::Real up, Ogre::Real down);
			void setAutoBox(bool automatic);

		protected:
			std::vector<std::pair<char,Ogre::Real> >	_gears;
			unsigned int					        	_current_gear_num;
			std::pair<char,Ogre::Real>		        	_current_gear;
			Ogre::Real*						        	_torque_curve;
			unsigned int					        	_curve_length;
			Ogre::Real						        	_rev_limit,_throttle_position,_brake_position,_wheel_rpm,_inv_rev_limit;
			Ogre::Real						        	_brake_force;
			Ogre::Real						        	_final_drive;
			Ogre::Real						        	_shift_up,_shift_down;
			bool							        	_auto_box;

			Vehicle*     mVehicle;
		};
	}
}