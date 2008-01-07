#ifndef __CylinderFactoryPrerequisites_H__
#define __CylinderFactoryPrerequisites_H__

#include <Fusion.h>

#include "Cylinder.h"
#include "CylinderFactory.h"

namespace Fusion 
{
#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(OGRE_STATIC_LIB)
#	ifdef CYLINDERFACTORY_EXPORTS
#		define _CylinderFactoryExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _CylinderFactoryExport
#       else
#    		define _CylinderFactoryExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#    define _CylinderFactoryExport  __attribute__ ((visibility("default")))
#else
#	define _CylinderFactoryExport
#endif	// OGRE_WIN32

}

#endif //__CylinderFactoryPrerequisites_H__
