#pragma once

#include "Prerequisites.h"

#define LUA_DEBUGHANDLE  "Fusion::Core::Debug*"

namespace Fusion
{
	namespace Core
	{		
		class Debug : public Ogre::Singleton<Debug>, public Ogre::FrameListener
		{
		public:
			Debug(Ogre::RenderWindow* window);
			virtual ~Debug(void);

			static Debug& getSingleton(void);
			static Debug* getSingletonPtr(void);

			virtual bool frameStarted(const Ogre::FrameEvent& evt);
			virtual bool frameEnded(const Ogre::FrameEvent& evt);

			void setText(const String& text);
			void clear(void);

			static int _text(lua_State* L);	
			static int _clear(lua_State* L);
			static int _console(lua_State* L);
			static int _statistics(lua_State* L);
			static int _contacts(lua_State* L);
			static int _geometries(lua_State* L);
		
			static int openLuaLib(lua_State *L);

		protected:
			static const luaL_Reg methods[];

		private:
			String mDebugText;
			Ogre::RenderWindow* mWindow;		
			Ogre::Overlay* mConsoleOverlay;
			Ogre::Overlay* mStatisticsOverlay;
			Ogre::OverlayElement* guiAvg;
			Ogre::OverlayElement* guiCurr;
			Ogre::OverlayElement* guiBest;
			Ogre::OverlayElement* guiWorst;
			Ogre::OverlayElement* guiTris;
			Ogre::OverlayElement* guiBatches;
			Ogre::OverlayElement* guiDbg;
		};
	}
}
