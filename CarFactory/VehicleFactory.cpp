#include "VehicleFactory.h"

#include "Vehicle.h"
#include "Engine.h"
#include "Wheel.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

VehicleFactory::VehicleFactory(Game* game) : ObjectFactory("Vehicle", game) 
{ 
	game->getScriptManager()->registerLuaLib(&Vehicle::openLuaLib);
}

VehicleFactory::~VehicleFactory(void) { }

Object* VehicleFactory::createInstance(lua_State* L, int luaReference, const String& name)
{	
	int idx = lua_gettop(L);
	const char* mesh = getTableStringField(L, idx, "mesh");
	
	assert(idx == lua_gettop(L));

	Vehicle* vehicle = new Vehicle(name, mGame, mWorld, mSpace);
	vehicle->setLuaReference(luaReference);

	Ogre::MeshManager::getSingleton().load(mesh, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
	vehicle->setEntity(mWorld->getSceneManager()->createEntity(vehicle->getName() +  "_Entity", mesh));
	vehicle->getEntity()->setCastShadows(true);
	vehicle->getTransNode()->attachObject(vehicle->getEntity());

	OgreOde::EntityInformer ei(vehicle->getEntity());
	Ogre::Vector3 box = ei.getSize();

	if (!vehicle->getGeometry()) 
		vehicle->setGeometry(new OgreOde::BoxGeometry(box, mWorld));
	vehicle->getGeometry()->setUserObject(vehicle);

	vehicle->setTransformGeometry(new OgreOde::TransformGeometry(mWorld, vehicle->getInternalSpace()));
	vehicle->getTransformGeometry()->setEncapsulatedGeometry(vehicle->getGeometry());
	vehicle->getTransformGeometry()->setBody(vehicle->getBody());
	vehicle->getTransformGeometry()->setUserObject(vehicle);

	// MASS
	lua_pushstring(L, "mass");
	lua_gettable(L, idx);
	if (lua_istable(L, -1))
	{
		lua_pushvalue(L, -1);
		int mass_table = lua_gettop(L);
		Ogre::Real mass = getTableNumberField(L, mass_table, "value");				
		OgreOde::BoxMass box_mass(mass, Ogre::Vector3(box.x,box.y,box.z));
		vehicle->getBody()->setMass(box_mass);

		Ogre::Vector3 offset = Ogre::Vector3::ZERO;
		offset.x = getTableNumberField(L, mass_table, "x");
		offset.y = getTableNumberField(L, mass_table, "y");
		offset.z = getTableNumberField(L, mass_table, "z");
		vehicle->setOffset(-offset);
	} 
	lua_settop(L, idx);

	assert(idx == lua_gettop(L));	

	// ENGINE
	lua_pushstring(L, "engine");
	lua_gettable(L, idx);
	if (lua_istable(L, -1))
	{
		lua_pushvalue(L, -1);
		int engine_table = lua_gettop(L);
		Ogre::Real redline = getTableNumberField(L, engine_table, "redline");
		Ogre::Real brake = getTableNumberField(L, engine_table, "brake");
		vehicle->getEngine()->setRevLimit(redline);
		vehicle->getEngine()->setBrakeForce(brake);

		lua_pushstring(L, "torque");
		lua_gettable(L, engine_table);
		if (lua_istable(L, -1))
		{
			lua_pushvalue(L, -1);
			int torque_table = lua_gettop(L);
			Ogre::Real torque_min = getTableNumberField(L, torque_table, "min");
			Ogre::Real torque_max = getTableNumberField(L, torque_table, "max");
			vehicle->getEngine()->setTorque(torque_min, torque_max);
		}

		const char* bufferName = getTableStringField(L, engine_table, "sound");		
		SoundBufferPtr sound = mGame->getSoundManager()->getByName(bufferName);
		if (!sound.isNull())
		{
			SoundSource* engineSound = mGame->getSoundManager()->createSoundSource(name + ".engineSound", sound.get(), Ogre::Vector3::ZERO);
			if (engineSound != NULL)
			{
				engineSound->setLoop(true);
				engineSound->setGain(0.0f);
				vehicle->setEngineSound(engineSound);
			}
		}
	} 	
	lua_settop(L, idx);	

	assert(idx == lua_gettop(L));

	// WHEELS
	lua_pushstring(L, "wheels");
	lua_gettable(L, idx);
	if (lua_istable(L, -1))
	{
		int wheels_table = lua_gettop(L);
		int count = luaL_getn(L, -1);
		for (int i = 1; i <= count; i++)
		{
			lua_rawgeti(L, -1, i);
			int wheel_table = lua_gettop(L);
			if (lua_istable(L, wheel_table))
			{
				const char* wheel_mesh = getTableStringField(L, wheel_table, "mesh");
				Ogre::Real wheel_mass = getTableNumberField(L, wheel_table, "mass");
				Ogre::Real wheel_break = getTableNumberField(L, wheel_table, "brake");				
				
				// WHEEL POSITION
				Ogre::Vector3 wheel_position = Ogre::Vector3::ZERO;
				lua_pushstring(L, "position");
				lua_gettable(L, wheel_table);
				if (lua_istable(L, -1))
				{
					lua_pushvalue(L, -1);
					int wheel_pos_table = lua_gettop(L);
					wheel_position.x = getTableNumberField(L, wheel_pos_table, "x");
					wheel_position.y = getTableNumberField(L, wheel_pos_table, "y");
					wheel_position.z = getTableNumberField(L, wheel_pos_table, "z");
				}
				Wheel* wheel = vehicle->addWheel(wheel_mesh, wheel_position, wheel_mass);
				wheel->setBrakeFactor(wheel_break);

				lua_settop(L, wheel_table);
				Ogre::Real wheel_power = getTableNumberField(L, wheel_table, "power");
				if (wheel_power > 0)
					wheel->setPowerFactor(wheel_power);
				
				// WHEEL STEER
				lua_pushstring(L, "steer");
				lua_gettable(L, wheel_table);
				if (lua_istable(L, -1))
				{
					lua_pushvalue(L, -1);
					int wheel_steer_table = lua_gettop(L);
					Ogre::Real factor = getTableNumberField(L, wheel_steer_table, "factor");
					Ogre::Real force = getTableNumberField(L, wheel_steer_table, "force");
					Ogre::Real speed = getTableNumberField(L, wheel_steer_table, "speed");
					Ogre::Real limit = getTableNumberField(L, wheel_steer_table, "limit");
					wheel->setSteerFactor(factor);
					wheel->setSteerForce(force);
					wheel->setSteerSpeed(speed);
					wheel->setSteerLimit(limit);
				}		

				lua_settop(L, wheel_table);

				// WHEEL CONTACT
				lua_pushstring(L, "contact");
				lua_gettable(L, wheel_table);
				if (lua_istable(L, -1))
				{
					lua_pushvalue(L, -1);
					int wheel_contact_table = lua_gettop(L);
					Ogre::Real bouncyness = getTableNumberField(L, wheel_contact_table, "bouncyness");
					Ogre::Real friction = getTableNumberField(L, wheel_contact_table, "friction");
					Ogre::Real fds = getTableNumberField(L, wheel_contact_table, "fds");
					wheel->setContact(bouncyness, friction, fds);
				}	
			}
			lua_settop(L, wheels_table);
		}
	}
	lua_settop(L, idx);	
	assert(idx == lua_gettop(L));

	// SUSPENSION
	lua_pushstring(L, "suspension");
	lua_gettable(L, idx);
	if (lua_istable(L, -1))
	{
		lua_pushvalue(L, -1);
		int suspension_table = lua_gettop(L);
		Ogre::Real spring  = getTableNumberField(L, suspension_table, "spring");
		Ogre::Real damping = getTableNumberField(L, suspension_table, "damping");
		Ogre::Real step    = getTableNumberField(L, suspension_table, "step");
		vehicle->setSuspension(spring, damping, step);
	} 	
	lua_settop(L, idx);	

	assert(idx == lua_gettop(L));

	/* table is in the stack at index 't' */
	lua_pushstring(L, "onCollision");
	lua_gettable(L, idx);
	if (lua_isfunction(L, -1))
	{
		vehicle->getTransformGeometry()->setUserData(OBJECT_ENABLE_COLLISION);
	}

	return vehicle;
}

void VehicleFactory::destroyInstance(Object* object)
{
	mSceneManager->destroySceneNode(object->getName());
	mSceneManager->destroyEntity(object->getName() + "_entity");	

	delete object;
}