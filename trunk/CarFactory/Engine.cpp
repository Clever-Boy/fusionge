#include "Engine.h"

#include "Vehicle.h"
#include "Wheel.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

Engine::Engine(Vehicle* vehicle) : _torque_curve(0), _curve_length (0), mVehicle(vehicle)
{
	setInputs(false,false);
	setRevLimit(40.0);
	setTorque(0.5,5.0);
	setBrakeForce(500.0);
}

Ogre::Real Engine::getPowerAtRPM(Ogre::Real rpm)
{
	_wheel_rpm = rpm;

	const Ogre::Real pos = getRevFactor() * (_curve_length - 1);
	const unsigned int i = (unsigned int)pos;
	const Ogre::Real dx = pos - ((Ogre::Real)i);

	assert (i < _curve_length);
	return _torque_curve[i] + (dx * (_torque_curve[i+1] - _torque_curve[i]));
}

Ogre::Real Engine::getBrakeForce()
{
	return _brake_force * _brake_position;
}

void Engine::setInputs(Ogre::Real throttle_position, Ogre::Real brake_position)
{
	_throttle_position = throttle_position;
	_brake_position = _brake_position;
}

void Engine::setInputs(Ogre::Real throttle_brake)
{
	_throttle_position = std::max((Ogre::Real)0.0,throttle_brake);
	_brake_position = std::max((Ogre::Real)0.0,-throttle_brake);
}

void Engine::setInputs(Ogre::Real time, bool throttle, bool brake)
{
	if (throttle) 
	{
		_throttle_position = std::min(1.0f, _throttle_position + (0.3f * time));
	} 
	else
	{
		_throttle_position = std::max(0.0f, _throttle_position - (0.5f * time));
	}
		
	if (brake) 
	{
		_brake_position = std::min(1.0f, _brake_position + (0.5f * time));
	} 
	else 
	{
		_brake_position = std::max(0.0f, _brake_position - (0.7f * time));
	}

	if (brake && throttle)
	{
		_throttle_position = 0;
		_brake_position = 0;
	}
}

// Manual gearbox
void Engine::changeUp()
{
}

void Engine::changeDown()
{
}

void Engine::setGear(char code)
{
}

void Engine::setGear(unsigned int i)
{
}

// Information functions
Ogre::Real Engine::getDesiredRPM()
{
	if (_throttle_position == 0 && _brake_position != 0) 
	{
		return - (_rev_limit/4 * _brake_position);
	}

	return _rev_limit * _throttle_position;
}

Ogre::Real Engine::getRevFactor()
{
	return _wheel_rpm * _inv_rev_limit;
}

char Engine::getGearCode()
{
	return '\0';
}

// Engine parameters
void Engine::setRevLimit(Ogre::Real rpm)
{
	_rev_limit = rpm;
	_inv_rev_limit = 1.0 / _rev_limit;
}

// Set a custom torque curve
void Engine::setTorque(Ogre::Real* torque, unsigned int samples)
{
	assert((samples > 1)&&("Need more than one sample for setTorque"));

	delete[] _torque_curve;
	_curve_length = samples;
	_torque_curve = new Ogre::Real[_curve_length + 1];

	for (unsigned int i = 0;i < samples;i++) 
		_torque_curve[i] = torque[i];
	_torque_curve[_curve_length] = torque[samples - 1];
}

// Set a linear torque curve, minimum should be greater than zero, 
// or the vehicle won't go anywhere!
void Engine::setTorque(Ogre::Real minimum,Ogre::Real maximum)
{
	Ogre::Real curve[] = {minimum,maximum};
	setTorque(curve, 2);
}

// Set a constant torque "curve"
void Engine::setTorque(Ogre::Real constant)
{
	setTorque(constant, constant);
}

// Braking parameters
void Engine::setBrakeForce(Ogre::Real force)
{
	_brake_force = force;
}

void Engine::update(Ogre::Real time)
{

}

// Drivetrain parameters
void Engine::addGear(Ogre::Real ratio,char code)
{
}

void Engine::setFinalDrive(Ogre::Real ratio)
{
}

void Engine::setAutoShiftRevs(Ogre::Real up, Ogre::Real down)
{
}

void Engine::setAutoBox(bool automatic)
{
}

Engine::~Engine()
{
	delete[] _torque_curve;
}