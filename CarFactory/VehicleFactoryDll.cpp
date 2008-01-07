#include "VehicleFactoryPrerequisites.h"

Fusion::Factories::VehicleFactory* factory;

//-----------------------------------------------------------------------
extern "C" void _VehicleFactoryExport dllStartFactory(Fusion::Core::Game* game)
{
    // Create factory	
	factory = new Fusion::Factories::VehicleFactory(game);

    // Register
	game->getObjectManager()->registerObjectFactory(factory);	
}

extern "C" void _VehicleFactoryExport dllStopFactory(Fusion::Core::Game* game)
{
	game->getObjectManager()->unregisterObjectFactory(factory);
    delete factory;
}