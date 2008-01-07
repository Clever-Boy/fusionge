#include "StableHeaders.h"
#include "ObjectManager.h"
#include <OgreDynLibManager.h>
#include <OgreDynLib.h>

using namespace Fusion::Core;

template<> ObjectManager* Ogre::Singleton<ObjectManager>::ms_Singleton = 0;

ObjectManager* ObjectManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

ObjectManager& ObjectManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );
}

typedef void (*DLL_START_FACTORY)(Game* game);
typedef void (*DLL_STOP_FACTORY)(Game* game);

ObjectManager::ObjectManager(Ogre::SceneManager* sceneManager)
{
	// Create the ODE world
	mWorld = new World(sceneManager);
	mWorld->setGravity(Ogre::Vector3(0,-9.80665, 0));
	mWorld->setCFM(10e-5);
	mWorld->setERP(0.8);
	mWorld->setAutoSleep(true);
    mWorld->setAutoSleepAverageSamplesCount(5);
	mWorld->setAutoSleepAngularThreshold(0.05);
	mWorld->setAutoSleepLinearThreshold(0.05);
	mWorld->setContactCorrectionVelocity(1.0);
	mSpace = mWorld->getDefaultSpace();

	// Create something that will step the world, but don't do it automatically    
	mStepper = new OgreOde::ForwardFixedStepHandler(mWorld,
										OgreOde::StepHandler::QuickStep, 0.01,
										Ogre::Real(1.0 / 4), Ogre::Real(1.7));

	mWorld->setCollisionListener(this);
}

ObjectManager::~ObjectManager(void) 
{
	delete mStepper;
	delete mWorld;
}

bool ObjectManager::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool ObjectManager::frameEnded(const Ogre::FrameEvent& evt)
{
	// Step the world and then synchronise the scene nodes with it, 
	// we could get this to do this automatically, but we 
	// can't be sure of what order the frame listeners will fire in
	if (mStepper->step(evt.timeSinceLastFrame))
    {
		std::vector<Object*>::iterator eraseIter = eraseQueue.begin();
		while (eraseIter != eraseQueue.end())
		{
			ObjectFactory* factory = this->getObjectFactory((*eraseIter)->getTypeName());
			assert(factory != NULL);
			factory->destroyInstance(*eraseIter);			
			eraseIter++;
		}		
		eraseQueue.clear();

		mWorld->synchronise(); 

		std::map<std::string, Object*>::iterator iter = objects.begin();
		while (iter != objects.end())
		{			
			if (iter->second != NULL)
			{
				iter->second->update(evt.timeSinceLastFrame);
			}
			iter++;
		}
    }

	return true;
}

void ObjectManager::registerObjectFactory(ObjectFactory* factory)
{
	this->factories[factory->getName().c_str()] = factory;
}

void ObjectManager::unregisterObjectFactory(ObjectFactory* factory)
{
	this->factories.erase(factory->getName());
}

ObjectFactory* ObjectManager::getObjectFactory(const String& name)
{
	return this->factories[name];
}

void ObjectManager::loadFactories(const String& factoriesfile)
{
	String pluginDir;
	Ogre::StringVector pluginList;	
	Ogre::ConfigFile cfg;
	
	try {
		cfg.load( factoriesfile );
	}
	catch (Ogre::Exception)
	{
		Ogre::LogManager::getSingleton().logMessage(factoriesfile + " not found, automatic object factories loading disabled.");
		return;
	}

	pluginDir = cfg.getSetting("ObjectFactoryFolder"); // Ignored on Mac OS X, uses Resources/ directory
	pluginList = cfg.getMultiSetting("ObjectFactory");

	char last_char = pluginDir[pluginDir.length()-1];
	if (last_char != '/' && last_char != '\\')
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		pluginDir += "\\";
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		pluginDir += "/";
#endif
	}

	for( Ogre::StringVector::iterator it = pluginList.begin(); it != pluginList.end(); ++it )
	{
		this->loadFactory(pluginDir + (*it));
	}

}

void ObjectManager::loadFactory(const String& factoryfile)
{
	// Load plugin library
	Ogre::DynLib* lib = Ogre::DynLibManager::getSingleton().load( factoryfile );
	// Store for later unload
	mFactoriesLibs.push_back(lib);

	// Call startup function
	DLL_START_FACTORY pFunc = (DLL_START_FACTORY)lib->getSymbol("dllStartFactory");

	if (!pFunc)
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Cannot find symbol dllStartFactory in library " + factoryfile,
		"ObjectManager::LoadFactory");

	// This must call installPlugin
	pFunc(Game::getSingletonPtr());
}

Object* ObjectManager::createInstance(lua_State* L, const String& className, const String& name)
{
	int luaReference = ScriptManager::getSingleton().getLuaClass(className);
	if (luaReference == 0) 
	{
		throw new Ogre::Exception(0, "Lua class '" + className + "' not found", __FILE__);
	}
	
	// põe a classe da pilha
	lua_getref(L, luaReference);
	int idx = lua_gettop(L);

	const String type(LuaHelper::getTableStringField(L, idx, "type"));

	ObjectFactory* factory = this->getObjectFactory(type);
	if (factory == NULL)
	{
		throw new Ogre::Exception(0, "no factory registered for type '" + type + "'", __FILE__);
	}

	Object* obj = factory->createInstance(L, luaReference, name);
	obj->setClassName(className);

	// register the object
	objects[name] = obj;

	return obj;
}
		
Object* ObjectManager::getInstance(const String& name)
{
	return objects[name];
}

void ObjectManager::destroyInstance(const String& name)
{
	Object* obj = objects[name];
	if (obj != NULL)
	{		
		eraseQueue.push_back(obj);
		objects.erase(name);
	}
}

bool ObjectManager::collision(OgreOde::Contact* contact)
{
	bool result = false;

	OgreOde::Geometry* firstGeometry = contact->getFirstGeometry();
	OgreOde::Geometry* secondGeometry = contact->getSecondGeometry();

	if (firstGeometry->getUserObject() != NULL 
		&& secondGeometry->getUserObject() != NULL)
	{
		if (firstGeometry->getUserData() == OBJECT_ENABLE_COLLISION)
		{
			result = ScriptManager::getSingleton().fireOnCollisionEvent(
				(Object*)firstGeometry->getUserObject(), 
				(Object*)secondGeometry->getUserObject());
		}
		
		if (!result && secondGeometry->getUserData() == OBJECT_ENABLE_COLLISION)
		{
			result = ScriptManager::getSingleton().fireOnCollisionEvent(
				(Object*)secondGeometry->getUserObject(), 
				(Object*)firstGeometry->getUserObject());
		}
	}

	if (!result) 
	{
		contact->setBouncyness(0.0);
		contact->setCoulombFriction(18.0);
	}

	return true;
}

World* ObjectManager::getWorld(void)
{
	return this->mWorld;
}

OgreOde::Space* ObjectManager::getSpace(void)
{
	return this->mSpace;
}
