#ifndef __Fusion_Core_Prerequisites_H__
#define __Fusion_Core_Prerequisites_H__

#include <lua.hpp>
#include <Ogre.h>
#include <OgreOde_Core.h>

#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

#include <al.h>
#include <alc.h>
#include <alut.h>

namespace Fusion
{
	namespace Core
	{
		#define LGE_VERSION_MAJOR 0
		#define LGE_VERSION_MINOR 1
		#define LGE_VERSION_PATCH 0
		#define LGE_VERSION_SUFFIX ""
		#define LGE_VERSION_NAME "Alpha"

		#define LGE_VERSION    ((LGE_VERSION_MAJOR << 16) | (LGE_VERSION_MINOR << 8) | LGE_VERSION_PATCH)

		#if LGE_WCHAR_T_STRINGS
			typedef std::wstring _StringBase;
		#else
			typedef std::string _StringBase;
		#endif

		typedef _StringBase String;

		class Game;
		class Camera;
		class Object;
		class ObjectFactory;
		class ObjectManager;
		class InputManager;
		class ScriptManager;
		class SoundBuffer;
		class SoundSource;
		class SoundManager;	
		class LuaHelper;
		class World;
		class Debug;
		class Light;
		class Geometry;
		class LoadingBar;
	}
}

#endif //__Fusion_Core_Prerequisites_H__