#include "StableHeaders.h"
#include "Game.h"
#include "OpenALSoundManager.h"

using namespace Fusion::Core;

#pragma region Singleton
template<> Game* Ogre::Singleton<Game>::ms_Singleton = 0;

Game* Game::getSingletonPtr(void)
{
	return ms_Singleton;
}

Game& Game::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );
}
#pragma endregion

Game::Game(void) : mSceneManager(0), mScriptManager(0), mCamera(0), mObjectManager(0), mDebug(0)
{	
	this->mRoot = new Ogre::Root("config.cfg");	
}

Game::~Game(void)
{	
	this->mRoot->removeFrameListener(mSoundManager);
	this->mRoot->removeFrameListener(mInputManager);
	this->mRoot->removeFrameListener(mObjectManager);
	this->mRoot->removeFrameListener(mCamera);	

	delete this->mScriptManager;
	delete this->mObjectManager;
	delete this->mSoundManager;
	delete this->mInputManager;	
	delete this->mCamera;
	delete this->mDebug;

	this->mRoot->destroySceneManager(mSceneManager);

	delete mRoot;
}

void Game::SetupResources(void)
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;

	cf.load("resources.cfg");

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);			
		}
	}
}

bool Game::Configure(void)
{
	bool result = false;

	if(this->mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		this->mWindow = mRoot->initialise(true, "Fusion Game Engine v0.1");	
		result =  true;
	}

	return result;
}

void Game::CreateViewports(void)
{
	Ogre::Camera* camera = mCamera->getOgreCamera();

	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	camera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

bool Game::start(const String& script)
{		
	this->SetupResources();

	if (!this->Configure())
	{
		return false;
	}

	this->mSceneManager = mRoot->createSceneManager(Ogre::ST_EXTERIOR_FAR);

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	this->mCamera = new Camera("Camera", mSceneManager);	
	CreateViewports();

	this->mScriptManager = new ScriptManager();
	this->mObjectManager = new ObjectManager(mSceneManager);
	this->mSoundManager = new OpenALSoundManager(mCamera);
	this->mInputManager = new InputManager(mWindow);	

	this->mRoot->addFrameListener(mSoundManager);
	this->mRoot->addFrameListener(mInputManager);
	this->mRoot->addFrameListener(mObjectManager);
	this->mRoot->addFrameListener(mCamera);	
		
	this->mInputManager->addKeyListener(mScriptManager, "ScriptManager");	

	// Register the object factories
	this->mObjectManager->loadFactories();
	
	Ogre::MovableObject::setDefaultQueryFlags(ANY_QUERY_MASK);
	
	this->mScriptManager->execute(script);		

	this->mRoot->startRendering();

	return true;
}

Camera* Game::getCamera(void)
{
	return this->mCamera;
}

Ogre::RenderWindow* Game::getWindow(void) 
{
	return this->mWindow;
}

Ogre::SceneManager* Game::getSceneManager(void)
{
	return this->mSceneManager;
}

ScriptManager* Game::getScriptManager(void)
{
	return this->mScriptManager;
}

ObjectManager* Game::getObjectManager(void)
{
	return this->mObjectManager;
}

SoundManager* Game::getSoundManager(void)
{
	return this->mSoundManager;
}

Debug* Game::getDebug(void)
{
	if (this->mDebug == NULL)
	{
		this->mDebug = new Debug(mWindow);
		this->mRoot->addFrameListener(mDebug);
	}

	return this->mDebug;
}

int Game::_actions(lua_State *L) 
{
	const char* type = luaL_checkstring(L, 1);
	if (!lua_istable(L, 2))
	{
		return luaL_typerror(L, 2, "a table is expected");
	}

	int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

	if (strcmp(type, "keyboard") == 0)
	{
		ScriptManager::getSingleton().setKeyboardActionMap(luaRef);
	}
	else if (strcmp(type, "mouse") == 0)
	{
		ScriptManager::getSingleton().setMouseActionMap(luaRef);
	} 
	else 
	{
		return luaL_error(L, "invalid input '%s'", type);
	}

	return 0;
}

int Game::_register(lua_State *L) 
{
	const char* name = luaL_checkstring(L, 1);
	if (!lua_istable(L, 2))
	{
		return luaL_typerror(L, 2, "a table is expected");
	}

	int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

	ScriptManager::getSingleton().registerLuaClass(name, luaRef);

	return 0;
}

int Game::_camera(lua_State* L)
{
	Camera **po = (Camera **)lua_newuserdata(L, sizeof(Camera *));
	*po = Game::getSingleton().getCamera();
	luaL_getmetatable(L, LUA_CAMERAHANDLE);
	lua_setmetatable(L, -2);

	return 1;
}

int Game::_world(lua_State* L)
{
	World **po = (World **)lua_newuserdata(L, sizeof(World *));
	*po = Game::getSingleton().getObjectManager()->getWorld();
	luaL_getmetatable(L, LUA_WORLDHANDLE);
	lua_setmetatable(L, -2);

	return 1;
}

int Game::_debug(lua_State* L)
{
	Debug **po = (Debug **)lua_newuserdata(L, sizeof(Debug *));
	Game::getSingleton().getDebug();
	*po = Debug::getSingletonPtr();
	luaL_getmetatable(L, LUA_DEBUGHANDLE);
	lua_setmetatable(L, -2);

	return 1;
}

int Game::_preload(lua_State* L)
{
	const char* name = luaL_checkstring(L, 1);
	String path(luaL_checkstring(L, 2));

	size_t idx = path.find_last_of('.');
	String extension = path.substr(idx + 1);
	if (extension.compare("wav") == 0) 
	{
		Ogre::NameValuePairList parameters;
		parameters["filename"] = path;

		Ogre::ResourceGroupManager::getSingleton().declareResource(name, "Sound", 
			SoundManager::SOUND_RESOURCE_GROUP_NAME, parameters);
	}
	
	lua_pushboolean(L, true);

	return 1;
}

int Game::_load(lua_State* L)
{
	Ogre::RenderWindow* window = Game::getSingleton().getWindow();
	
	LoadingBar bar;
	bar.start(window);	
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(SoundManager::SOUND_RESOURCE_GROUP_NAME);
	bar.finish();

	return 0;
}

int Game::_exit(lua_State* L)
{
	exit(0);
	return 0;
}

const luaL_Reg Game::methods[] = {
	{"actions", &Game::_actions},
	{"camera", &Game::_camera}, 
	{"world", &Game::_world}, 
	{"debug", &Game::_debug}, 
	{"register", &Game::_register},
	{"preload", &Game::_preload},
	{"load", &Game::_load},
	{"exit", &Game::_exit},
	{NULL, NULL}
};

int Game::openLuaLib(lua_State *L)
{
	luaL_register(L, LUA_GAMELIBNAME, methods);
	return 1;
}