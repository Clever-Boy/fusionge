#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		enum QueryFlags
		{
			ANY_QUERY_MASK					= 1<<0,
			ZOMBIE_QUERY_MASK				= 1<<1,
			GEOMETRY_QUERY_MASK				= 1<<2,
			VEHICLE_QUERY_MASK				= 1<<3,
			STATIC_GEOMETRY_QUERY_MASK		= 1<<4
		};

		class ObjectManager : public Ogre::Singleton<ObjectManager>, 
			public Ogre::FrameListener, public OgreOde::CollisionListener
		{
		public:
			ObjectManager(Ogre::SceneManager* sceneManager);
			~ObjectManager(void);

			static ObjectManager& getSingleton(void);
			static ObjectManager* getSingletonPtr(void);

			virtual bool frameStarted(const Ogre::FrameEvent& evt);
			virtual bool frameEnded(const Ogre::FrameEvent& evt);

			void registerObjectFactory(ObjectFactory* factory);
			void unregisterObjectFactory(ObjectFactory* factory);
			ObjectFactory* getObjectFactory(const String& name);

			Object* createInstance(lua_State* L, const String& className, const String& name);	
			Object* getInstance(const String& name);
			void destroyInstance(const String& name);		

			void loadFactories(const String& factoriesfile = "config.cfg");
			void loadFactory(const String& factoryfile);
			
			bool collision(OgreOde::Contact* contact);

			World* getWorld(void);
			OgreOde::Space* getSpace(void);

		protected:
			std::map<String, ObjectFactory*> factories;
			std::map<String, Object*> objects;
			std::vector<Object*> eraseQueue;

			/// List of plugin DLLs loaded
			std::vector<Ogre::DynLib*> mFactoriesLibs;

			World* mWorld;
			OgreOde::Space* mSpace;
			OgreOde::StepHandler* mStepper;
		};
	}
}