#pragma once

#include "Prerequisites.h"

#define LUA_GAMELIBNAME	"game"

namespace Fusion
{
	namespace Core
	{		
		class Game : public Ogre::Singleton<Game>
		{
		public:
			Game(void);
			~Game(void);

			static Game& getSingleton(void);
			static Game* getSingletonPtr(void);

			bool start(const String& script);

			void setShowDebug(bool show);
			bool isShowDebug(void);
			void setDebugText(const String& text);
			
			Ogre::RenderWindow* getWindow(void);
			Ogre::SceneManager* getSceneManager(void);				
			ScriptManager*      getScriptManager(void);
			ObjectManager*      getObjectManager(void);
			SoundManager*       getSoundManager(void);
			Camera*             getCamera(void);
			Debug*              getDebug(void);

			static int _camera(lua_State* L);
			static int _register(lua_State* L);
			static int _actions(lua_State* L);	
			static int _world(lua_State* L);	
			static int _debug(lua_State* L);	
			static int _preload(lua_State* L);
			static int _load(lua_State* L);
			static int _exit(lua_State* L);

			static int openLuaLib(lua_State *L);

		protected:
			void SetupResources(void);
			bool Configure(void);
			void CreateViewports(void);
			
		protected:
			ScriptManager*      mScriptManager;
			InputManager*       mInputManager;
			ObjectManager*      mObjectManager;
			SoundManager*       mSoundManager;
			Camera*             mCamera;		
			Debug*              mDebug;

			Ogre::Root*         mRoot;		
			Ogre::RenderWindow* mWindow;
			Ogre::SceneManager* mSceneManager;

			static const luaL_Reg methods[];
		};
	}
}
