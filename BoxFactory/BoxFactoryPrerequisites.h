#ifndef __BoxFactoryPrerequisites_H__
#define __BoxFactoryPrerequisites_H__

#include <Fusion.h>

#include "Box.h"
#include "BoxFactory.h"

namespace LGE 
{
#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(OGRE_STATIC_LIB)
#	ifdef BOXFACTORY_EXPORTS
#		define _BoxFactoryExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _BoxFactoryExport
#       else
#    		define _BoxFactoryExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#    define _BoxFactoryExport  __attribute__ ((visibility("default")))
#else
#	define _BoxFactoryExport
#endif	// OGRE_WIN32

}


#endif
