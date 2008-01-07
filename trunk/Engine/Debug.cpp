#include "StableHeaders.h"

#include "Debug.h"

using namespace Fusion::Core;

#pragma region Singleton
template<> Debug* Ogre::Singleton<Debug>::ms_Singleton = 0;

Debug* Debug::getSingletonPtr(void)
{
	return ms_Singleton;
}

Debug& Debug::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );
}
#pragma endregion

Debug::Debug(Ogre::RenderWindow* window) : mWindow(window)
{
	mConsoleOverlay = Ogre::OverlayManager::getSingleton().getByName("LGE/ConsoleOverlay");	
	mConsoleOverlay->hide();

	mStatisticsOverlay = Ogre::OverlayManager::getSingleton().getByName("LGE/StatisticsOverlay");	
	mStatisticsOverlay->hide();

	guiAvg = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/AverageFps");
	guiCurr = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/CurrFps");
	guiBest = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/BestFps");
	guiWorst = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/WorstFps");

	guiTris = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/NumTris");
	guiBatches = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/NumBatches");
	guiDbg = Ogre::OverlayManager::getSingleton().getOverlayElement("LGE/ConsoleText");

}

Debug::~Debug(void)
{
	Ogre::OverlayManager::getSingleton().destroy(mConsoleOverlay);
	Ogre::OverlayManager::getSingleton().destroy(mStatisticsOverlay);
}

bool Debug::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool Debug::frameEnded(const Ogre::FrameEvent& evt)
{
	static Ogre::String currFps   = "Current FPS: ";
	static Ogre::String avgFps    = "Average FPS: ";
	static Ogre::String bestFps   = "Best FPS: ";
	static Ogre::String worstFps  = "Worst FPS: ";
	static Ogre::String tris      = "Triangle Count: ";
	static Ogre::String batches   = "Batch Count: ";

	// update stats when necessary
	try 
	{
		const Ogre::RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + Ogre::StringConverter::toString(stats.bestFPS)
			+" "+ Ogre::StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + Ogre::StringConverter::toString(stats.worstFPS)
			+" "+ Ogre::StringConverter::toString(stats.worstFrameTime)+" ms");

		guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));
		guiBatches->setCaption(batches + Ogre::StringConverter::toString(stats.batchCount));
		guiDbg->setColour(Ogre::ColourValue(1));
		guiDbg->setCaption(mDebugText);
	}
	catch(...) { /* ignore */ }

	return true;
}

void Debug::setText(const String& text)
{
	static String lines[10];
	static int index = -1;
	static int start = 0;

	struct tm *pTime;
	time_t ctTime; time(&ctTime);
	pTime = localtime( &ctTime );
	std::ostringstream message;
	
	message << std::setw(2) << std::setfill('0') << pTime->tm_hour
	<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
	<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec 
	<< ": " << text << std::endl;

	lines[++index % 10] = message.str();

	this->mDebugText = "";

	for (int i = 0; i < 10 && index - i >= 0; i++)
	{
		this->mDebugText = lines[(index - i) % 10] + this->mDebugText;
	}
}

void Debug::clear(void)
{
	mDebugText = "";
}

#define topDebug(L)	((Debug **)lua_touserdata(L, 1))

int Debug::_text(lua_State* L)
{
	Debug* debug = *topDebug(L);

	const char *text = luaL_checkstring(L, 2);
	if (text != NULL)
	{
		debug->setText(text);
	}

	return 0;
}

int Debug::_clear(lua_State* L)
{
	Debug* debug = *topDebug(L);
	debug->clear();
	return 0;
}

int Debug::_console(lua_State* L)
{
	Debug* debug = *topDebug(L);

	if (lua_isnoneornil(L, 2)) 
	{
		bool result = debug->mConsoleOverlay->isVisible();
		lua_pushboolean(L, result);
		return 1;
	}
	else 
	{
		int value = lua_toboolean(L, 2);
		if (value != 0)
			debug->mConsoleOverlay->show();
		else
			debug->mConsoleOverlay->hide();

		return 0;
	}
}

int Debug::_statistics(lua_State* L)
{
	Debug* debug = *topDebug(L);

	if (lua_isnoneornil(L, 2)) 
	{
		bool result = debug->mStatisticsOverlay->isVisible();
		lua_pushboolean(L, result);
		return 1;
	}
	else 
	{
		int value = lua_toboolean(L, 2);
		if (value != 0)
			debug->mStatisticsOverlay->show();
		else
			debug->mStatisticsOverlay->hide();
		
		return 0;
	}
}

int Debug::_contacts(lua_State* L)
{
	if (lua_isnoneornil(L, 2)) 
	{
		bool result = ObjectManager::getSingleton().getWorld()->getShowDebugContact();
		lua_pushboolean(L, result);
		return 1;
	}
	else 
	{
		int value = lua_toboolean(L, 2);
		ObjectManager::getSingleton().getWorld()->setShowDebugContact(value != 0);
		return 0;
	}
}

int Debug::_geometries(lua_State* L)
{
	if (lua_isnoneornil(L, 2)) 
	{
		bool result = ObjectManager::getSingleton().getWorld()->getShowDebugGeometries();
		lua_pushboolean(L, result);
		return 1;
	}
	else 
	{
		int value = lua_toboolean(L, 2);
		ObjectManager::getSingleton().getWorld()->setShowDebugGeometries(value != 0);
		return 0;
	}
}

const luaL_Reg Debug::methods[] = {
	{"text", &Debug::_text},
	{"clear", &Debug::_clear},
	{"console", &Debug::_console},
	{"statistics", &Debug::_statistics},
	{"geometries", &Debug::_geometries},
	{"contacts", &Debug::_contacts},
	{NULL, NULL}
};

int Debug::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_DEBUGHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}
