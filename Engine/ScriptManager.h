#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class ScriptManager : public Ogre::Singleton<ScriptManager>, public OIS::KeyListener
		{
		public:
			ScriptManager(void);
			~ScriptManager(void);

			static ScriptManager& getSingleton(void);
			static ScriptManager* getSingletonPtr(void);

			bool execute(const String& file);

			void registerLuaLib(lua_CFunction func);
			void registerLuaClass(const String& name, int luaRef);
			int getLuaClass(const String& name);

			void setKeyboardActionMap(int luaRef);
			void setMouseActionMap(int luaRef);
			virtual bool keyPressed( const OIS::KeyEvent &arg );
			virtual bool keyReleased( const OIS::KeyEvent &arg );

			bool fireOnCollisionEvent(Object* objA, Object* objB);
			bool fireOnKeyEvent(const OIS::KeyEvent &arg, bool pressed);		

		public:
			lua_State* L;

		private:
			std::vector<lua_CFunction> luaLibs;
			std::map<String, int> luaClasses;
			int keyboardActionMap;
			int mouseActionMap;		
		};
	}
}