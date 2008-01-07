#include "Vehicle.h"

#include "Engine.h"
#include "Wheel.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

static float sIdleEngineVolume = 0.3;

Vehicle::Vehicle(const String& name,
				 const String& mesh, 
				 Ogre::Real mass, 
                 Game* game,
				 World* world,
				 OgreOde::Space* space,
                 const Ogre::Vector3& offset,
                 OgreOde::Geometry* geometry):
	Object(game, world, space, name),
    _antisway (false),
    _swayRate (Ogre::Real(0.0)),
    _swayLastUpdate (Ogre::Real(0.0)),
    _swayForce (Ogre::Real(0.0)),
    _swayForceLimit (0),
	leftPressed(0),
	rightPressed(0),
	throttlePressed(0),
	brakePressed(0)
{
	
	mEntity = mWorld->getSceneManager()->createEntity(name + "_Entity", mesh);
	mEntity->setCastShadows(true);
	
	mNode = mWorld->getSceneManager()->getRootSceneNode()->createChildSceneNode(name + "_Node");
	mBody = new OgreOde::Body(mWorld, name + "mBody");

	_trans_node = mNode->createChildSceneNode("Trans_" + mNode->getName());
	_trans_node->attachObject(mEntity);
	mNode->attachObject(mBody);

	OgreOde::EntityInformer ei(mEntity);
	Ogre::Vector3 box = ei.getSize();
	
	OgreOde::BoxMass box_mass(mass, Ogre::Vector3(box.x,box.y,box.z));
	mBody->setMass(box_mass);

	iSpace = new OgreOde::SimpleSpace(mWorld, space ? space : mWorld->getDefaultSpace());
	iSpace->setInternalCollisions(false);
	iSpace->setAutoCleanup(false);

	_offset = -offset;

	if (!geometry) 
		mGeometry = new OgreOde::BoxGeometry(box, mWorld, space ? space : mWorld->getDefaultSpace());
	else 
		mGeometry = geometry;
	mGeometry->setUserObject(this);	

	_transform = new OgreOde::TransformGeometry(mWorld, iSpace);
	_transform->setEncapsulatedGeometry(mGeometry);
	_transform->setBody(mBody);	

	mGeometry->setPosition(_offset);

	_trans_node->setPosition(_offset);

	mEngine = new Engine(this);
}

Vehicle::Vehicle(const String& name, 
				 Game* game, 
				 World* world,
				 OgreOde::Space* space,
				 OgreOde::Geometry* geometry) : 
        Object(game, world, space, geometry, name),
        _antisway(false),
        _swayRate (Ogre::Real(0.0)),
        _swayLastUpdate (Ogre::Real(0.0)),
        _swayForce (Ogre::Real(0.0)),
        _swayForceLimit (0),
        _transform (0),
		_offset (Ogre::Vector3::ZERO),
		leftPressed(0),
		rightPressed(0),
		throttlePressed(0),
		brakePressed(0)
{

	iSpace = new OgreOde::SimpleSpace(mWorld, space ? space : mWorld->getDefaultSpace());
	iSpace->setInternalCollisions(false);
	iSpace->setAutoCleanup(false);

	mEngine = new Engine(this);

	mNode = mWorld->getSceneManager()->getRootSceneNode()->createChildSceneNode(name + "_Node");
	mBody = new OgreOde::Body(mWorld, name + "mBody");

	_trans_node = mNode->createChildSceneNode("Trans_" + mNode->getName());
	mNode->attachObject(mBody);

	if (mGeometry) 
		mGeometry->setUserObject(this);
	
}

void Vehicle::setSleep(bool value)
{
	Object::setSleep(value);

	for (std::vector<Wheel*>::iterator i = _wheels.begin();i != _wheels.end();i++)
	{
		(*i)->setSleep(value);
	}
}

Wheel* Vehicle::addWheel(const String& mesh, const Ogre::Vector3& position, Ogre::Real mass)
{
	const String wheelName(mName + "_Wheel" + 
		Ogre::StringConverter::toString((unsigned int)_wheels.size() + 1));

	Wheel* wheel = new Wheel(this, wheelName, mesh, position + mBody->getPosition() + _offset,
		mass, mGame, mWorld, iSpace);
	_wheels.push_back(wheel);
	return wheel;
}

const Ogre::Vector3& Vehicle::getPosition()
{
    _lastPosition = mBody->getPosition() + _offset;
	return _lastPosition;
}

void Vehicle::setPosition(const Ogre::Vector3& position)
{
	for (std::vector<Wheel*>::iterator i = _wheels.begin();i != _wheels.end();i++)
	{
		Ogre::Vector3 diff = (*i)->getPosition() - mBody->getPosition();
		(*i)->setPosition(position + diff);
	}
	mBody->setPosition(position);
}

void Vehicle::setSuspension(Ogre::Real spring_constant, Ogre::Real damping_constant, Ogre::Real step_size)
{
	for (std::vector<Wheel*>::iterator i = _wheels.begin();i != _wheels.end();i++)
	{
		(*i)->setSuspension(spring_constant,damping_constant,step_size);
	}
}

void Vehicle::setInputs(Ogre::Real time, bool left,bool right,bool throttle,bool brake)
{
    if (left || right || throttle)
    {
		setSleep(false);
    }
	mEngine->setInputs(time, throttle, brake);
	setSteering(0.0 + ((right)?1.0:0.0) - ((left)?1.0:0.0));
}

void Vehicle::setInputs(Ogre::Real steering_position, Ogre::Real throttle_brake)
{
	mEngine->setInputs(throttle_brake);
	setSteering(steering_position);
}

void Vehicle::setInputs(Ogre::Real steering_position, Ogre::Real throttle_position, Ogre::Real brake_position)
{
	mEngine->setInputs(throttle_position, brake_position);
	setSteering(steering_position);
}

void Vehicle::setSteering(Ogre::Real steering_position)
{
	for (std::vector<Wheel*>::iterator i = _wheels.begin();i != _wheels.end();i++)
	{
		(*i)->setSteer(steering_position);
	}
}

Ogre::Real Vehicle::getVelocity(void)
{
	return mBody->getLinearVelocity().length();
}

void Vehicle::applyAntiSwayBarForces(void)
{
	Ogre::Real amt;
	OgreOde::Body * wheelBody;
	for( int i = 0; i < 4; i++)
	{
		OgreOde::SuspensionJoint * const wheelJoint = static_cast<OgreOde::SuspensionJoint*>( _wheels.at( i )->getBody()->getJoint(0) );
		const Ogre::Vector3 anchor2 (wheelJoint->getAdditionalAnchor());
		const Ogre::Vector3 anchor1 (wheelJoint->getAnchor());
		const Ogre::Vector3 axis (wheelJoint->getAxis());
		const Ogre::Real displacement = ( anchor1 - anchor2 ).dotProduct( axis );
		if( displacement > 0 )
		{
			amt = displacement * _swayForce;
			if( amt > _swayForceLimit ) 
				amt = _swayForceLimit;

			// force down
			wheelBody = _wheels.at( i )->getBody();
			wheelBody->addForce( -axis * amt );

			// force up
			wheelBody = _wheels.at( i^1 )->getBody();
			wheelBody->addForce( axis * amt );
		}
	}
}

void Vehicle::update(Ogre::Real time)
{
	setInputs(time, leftPressed, rightPressed, throttlePressed, brakePressed);

	std::vector<Wheel*>::iterator b = _wheels.begin();
	std::vector<Wheel*>::iterator e = _wheels.end();

	Ogre::Real rpm = FLT_MIN;

	for (std::vector<Wheel*>::iterator i = b;i != e;i++)
	{
		rpm = std::max(rpm,(*i)->getRPM());
	}

	mEngine->update(time);
	Ogre::Real power = mEngine->getPowerAtRPM(rpm);
	Ogre::Real desired_rpm = mEngine->getDesiredRPM();
	Ogre::Real brake = mEngine->getBrakeForce();

	for (std::vector<Wheel*>::iterator i = b;i != e;i++)
	{
		(*i)->update(power, desired_rpm, brake);
	}
	
	if (_antisway)
	{
		_swayLastUpdate += time;
		if  (_swayLastUpdate > _swayRate)
		{
			applyAntiSwayBarForces ();
			_swayLastUpdate =Ogre::Real(0.0);
		}
	}

	updateEngineSound(sIdleEngineVolume + (1 - sIdleEngineVolume) * (1 - desired_rpm/100.0f));
}

void Vehicle::updateEngineSound(float level)
{
   if (mEngineSound != NULL) 
   {
	   mEngineSound->setPosition(getPosition());
       mEngineSound->setGain(level);

       // Creative Labs win32 OpenAL workaround
       float pitch = ((level-sIdleEngineVolume) * 1.3);
       if (pitch < 0.5)  
          pitch = 0.5;
       // End workaround

      mEngineSound->setPitch(pitch);
   }
}

Wheel* Vehicle::findWheel(OgreOde::Geometry* geometry)
{
	for (std::vector<Wheel*>::iterator i = _wheels.begin();i != _wheels.end();i++)
	{
		if ((*i)->getGeometry() == geometry)
			return (*i);
	}
	return 0;
}

Vehicle::~Vehicle()
{
	// _world->unregisterVehicle(_name);

	for (std::vector<Wheel*>::iterator i = _wheels.begin();i != _wheels.end();i++) 
		delete (*i);

	delete mBody;
	delete mGeometry;
	delete _transform;
	delete iSpace;
	delete mEngine;

	mNode->removeAndDestroyChild(_trans_node->getName());
	mWorld->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(mNode->getName());
	mWorld->getSceneManager()->destroyEntity(mEntity->getName());
}

bool Vehicle::handleCollision(OgreOde::Contact* contact)
{
	OgreOde::Geometry *geom1 = contact->getFirstGeometry();
	OgreOde::Geometry *geom2 = contact->getSecondGeometry();
	Object* obj1 = (Object*) geom1->getUserObject();
	Object* obj2 = (Object*) geom2->getUserObject();

	if (obj1 && obj1->getTypeName().compare("Wheel") == 0)
	{
		((Wheel*)(obj1))->setupTyreContact(contact);
		return true;
	}
	else if (obj2 && obj2->getTypeName().compare("Wheel") == 0)
	{
		((Wheel*)(obj2))->setupTyreContact(contact);
		return true;
	}		

	return Object::handleCollision(contact);
}

const luaL_Reg Vehicle::methods[] = {
	{"left", &Vehicle::_left},
	{"right", &Vehicle::_right}, 
	{"throttle", &Vehicle::_throttle},
	{"brake", &Vehicle::_brake},
	{NULL, NULL}
};

int Vehicle::openLuaLib(lua_State *L)
{
	return Object::openLuaLib(L, LUA_VEHICLEHANDLE, Vehicle::methods);
}

int Vehicle::_left(lua_State *L)
{
	Vehicle** v = (Vehicle**)lua_touserdata(L, 1);

	if (lua_isboolean(L, 2))
	{
		(*v)->leftPressed = lua_toboolean(L, 2) != 0;
	}	

	return 0;
}

int Vehicle::_right(lua_State *L)
{
	Vehicle** v = (Vehicle**)lua_touserdata(L, 1);

	if (lua_isboolean(L, 2))
	{
		(*v)->rightPressed = lua_toboolean(L, 2) != 0;
	}	

	return 0;
}

int Vehicle::_throttle(lua_State *L)
{
	Vehicle** v = (Vehicle**)lua_touserdata(L, 1);

	if (lua_isboolean(L, 2))
	{
		(*v)->throttlePressed = lua_toboolean(L, 2) != 0;
	}	

	return 0;
}

int Vehicle::_brake(lua_State *L)
{
	Vehicle** v = (Vehicle**)lua_touserdata(L, 1);

	if (lua_isboolean(L, 2))
	{
		(*v)->brakePressed = lua_toboolean(L, 2) != 0;
	}	

	return 0;
}