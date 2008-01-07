#include "SphereFactoryPrerequisites.h"

Fusion::Factories::SphereFactory* factory;

//-----------------------------------------------------------------------
extern "C" void _SphereFactoryExport dllStartFactory(Fusion::Core::Game* game)
{
    // Create factory	
	factory = new Fusion::Factories::SphereFactory(game);

    // Register
	game->getObjectManager()->registerObjectFactory(factory);
}

extern "C" void _SphereFactoryExport dllStopFactory(Fusion::Core::Game* game)
{
	game->getObjectManager()->unregisterObjectFactory(factory);
    delete factory;
}