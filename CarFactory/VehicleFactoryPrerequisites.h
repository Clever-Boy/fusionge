#ifndef __CarFactoryPrerequisites_H__
#define __CarFactoryPrerequisites_H__

#include <Fusion.h>

#include "Vehicle.h"
#include "VehicleFactory.h"

namespace Fusion 
{
    
#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(OGRE_STATIC_LIB)
#	ifdef VEHICLEFACTORY_EXPORTS
#		define _VehicleFactoryExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _VehicleFactoryExport
#       else
#    		define _VehicleFactoryExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#    define _VehicleFactoryExport  __attribute__ ((visibility("default")))
#else
#	define _VehicleFactoryExport
#endif	// OGRE_WIN32


}


#endif
