#include "Wheel.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

const Ogre::Real Wheel::FudgeFactor = 0.1;

Wheel::Wheel(Vehicle* vehicle, const String& name, const String& mesh, 
		     const Ogre::Vector3& position, Ogre::Real mass, Game* game, World *world, 
			 OgreOde::Space* space) : Object(game, world, space, name)
{
	_owner = vehicle;

	mEntity = mWorld->getSceneManager()->createEntity(name + "mEntity", mesh);
	mEntity->setCastShadows(true);	

	OgreOde::EntityInformer ei(mEntity);
	_radius = ei.getRadius();

	mNode = mWorld->getSceneManager()->getRootSceneNode()->createChildSceneNode(name + "mNode");
	mNode->setPosition(position);
	mBody = new OgreOde::Body(mWorld, name + "_Body");

	mNode->attachObject(mEntity);
	mNode->attachObject(mBody);

	mBody->setMass(OgreOde::SphereMass(mass,_radius));

	mGeometry = new OgreOde::SphereGeometry(_radius, mWorld, space);
	mGeometry->setBody(mBody);
	mGeometry->setUserObject(this);

	mJoint = new OgreOde::SuspensionJoint(mWorld);
	mJoint->attach(vehicle->getBody(),mBody);
	mJoint->setAnchor(mBody->getPosition());
	mJoint->setAxis(Ogre::Vector3::UNIT_Y);
	mJoint->setAdditionalAxis(Ogre::Vector3::NEGATIVE_UNIT_X);
	mJoint->setParameter(OgreOde::Joint::Parameter_FudgeFactor, Wheel::FudgeFactor);
	
	_bouncyness = _friction = _fds = 0.0;

	setSteerFactor(0.0);
	setSteerLimit(0.0);
	setSteerForce(0.0);
	setSteerSpeed(0.0);
			
	setPowerFactor(0.0);
			
	setBrakeFactor(0.0);

	setSteer(0.0);
}

void Wheel::setFactors(Ogre::Real steer, Ogre::Real power, Ogre::Real brake)
{
	setSteerFactor(steer);
	setPowerFactor(power);
	setBrakeFactor(brake);
}

Ogre::Real Wheel::getRPM()
{
	return ((mJoint)&&(_power_factor > 0.0))?mJoint->getParameter(OgreOde::Joint::Parameter_MotorVelocity,2):0.0;
}

Ogre::Real Wheel::getSteer()
{
	return ((mJoint)&&(_steer_factor != 0.0))?mJoint->getAngle():0.0;
}

void Wheel::setSteerFactor(Ogre::Real factor)
{
	_steer_factor = factor;
	adjustJointParameters();
}

void Wheel::setSteerLimit(Ogre::Real limit)
{
	_steer_limit = limit;
	adjustJointParameters();
}

void Wheel::adjustJointParameters()
{
	if (mJoint)
	{
		mJoint->setParameter(OgreOde::Joint::Parameter_HighStop,_steer_limit * fabs(_steer_factor));
		mJoint->setParameter(OgreOde::Joint::Parameter_LowStop,-(_steer_limit * fabs(_steer_factor)));
	}
}


//------------------------------------------------------------------------------------------------
/* 
According to the ODE docs;

By adjusting the values of ERP and CFM, you can achieve various effects. 
For example you can simulate springy constraints, where the two bodies oscillate 
as though connected by springs. Or you can simulate more spongy constraints, without 
the oscillation. In fact, ERP and CFM can be selected to have the same effect as any 
desired spring and damper constants. If you have a spring constant kp and damping constant kd, 
then the corresponding ODE constants are:

ERP = h kp / (h kp + kd)
CFM = 1 / (h kp + kd)

where h is the stepsize. These values will give the same effect as a spring-and-damper 
system simulated with implicit first order integration.
*/
void Wheel::setSuspension(Ogre::Real spring_constant, Ogre::Real damping_constant, Ogre::Real step_size)
{
	_spring = spring_constant;
	_damping = damping_constant;
	_step = step_size;

	if (mJoint)
	{
		mJoint->setParameter(OgreOde::Joint::Parameter_SuspensionERP,_step * _spring / (_step * _spring + _damping));
		mJoint->setParameter(OgreOde::Joint::Parameter_SuspensionCFM,1.0 / (_step * _spring + _damping));
	}
}

void Wheel::update(Ogre::Real power_force, Ogre::Real desired_rpm, Ogre::Real brake_force)
{
	if (!mJoint) return;

	Ogre::Real force = (power_force * _power_factor) - (brake_force * _brake_factor);

	if (force > 0.0)
	{
		mJoint->setParameter(OgreOde::Joint::Parameter_MotorVelocity,desired_rpm,2);
		mJoint->setParameter(OgreOde::Joint::Parameter_MaximumForce,force,2);
	}
	else
	{
		mJoint->setParameter(OgreOde::Joint::Parameter_MotorVelocity,desired_rpm,2);
		mJoint->setParameter(OgreOde::Joint::Parameter_MaximumForce,-force,2);
	}
}

void Wheel::setSteer(Ogre::Real position)
{
	Ogre::Real steer_angle = position * _steer_limit;

	// Steer
	if (_steer_limit * _steer_factor != 0.0)
	{
		Ogre::Real vel = (steer_angle * _steer_factor) - mJoint->getAngle();
		vel *= (_steer_speed * fabs(_steer_factor));

		mJoint->setParameter(OgreOde::Joint::Parameter_MotorVelocity,vel);
		mJoint->setParameter(OgreOde::Joint::Parameter_MaximumForce,_steer_force);
	}
}

void Wheel::snapOff()
{
	delete mJoint;
	mJoint = 0;
}

void Wheel::setupTyreContact(OgreOde::Contact* contact)
{
	Ogre::Quaternion vehicle_orient = _owner->getBody()->getOrientation();

	Ogre::Vector3 wheel_up = vehicle_orient * Ogre::Vector3::UNIT_Y;
	Ogre::Quaternion wheel_rot(Ogre::Radian(getSteer()), (Ogre::Vector3) wheel_up);
	Ogre::Vector3 wheel_forward = wheel_rot * (vehicle_orient * Ogre::Vector3::UNIT_Z);
	Ogre::Vector3 wheel_velocity = mBody->getLinearVelocity();

	contact->setFrictionMode(OgreOde::Contact::Flag_BothFrictionPyramids);
	contact->setBouncyness(_bouncyness);
	contact->setCoulombFriction(_friction);
	contact->setFirstFrictionDirection(wheel_forward);
	contact->setAdditionalFDS(wheel_velocity.length() * _fds);
}

Wheel::~Wheel()
{
	delete mJoint;
	delete mBody;
	delete mGeometry;

	mWorld->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(mNode->getName());
	mWorld->getSceneManager()->destroyEntity(mEntity->getName());
}

