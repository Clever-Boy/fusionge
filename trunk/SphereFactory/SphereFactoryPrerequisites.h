#ifndef __SphereFactoryPrerequisites_H__
#define __SphereFactoryPrerequisites_H__

#include <Fusion.h>

#include "Sphere.h"
#include "SphereFactory.h"

namespace Fusion 
{
    
#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(OGRE_STATIC_LIB)
#	ifdef SPHEREFACTORY_EXPORTS
#		define _SphereFactoryExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _SphereFactoryExport
#       else
#    		define _SphereFactoryExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#    define _SphereFactoryExport  __attribute__ ((visibility("default")))
#else
#	define _SphereFactoryExport
#endif	// OGRE_WIN32


}


#endif
