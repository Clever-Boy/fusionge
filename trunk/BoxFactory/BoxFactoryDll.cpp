
#include "BoxFactoryPrerequisites.h"

Fusion::Factories::BoxFactory* boxFactory;

//-----------------------------------------------------------------------
extern "C" void _BoxFactoryExport dllStartFactory(Fusion::Core::Game* game)
{
    // Create factory	
	boxFactory = new Fusion::Factories::BoxFactory(game);

    // Register
	game->getObjectManager()->registerObjectFactory(boxFactory);
}

extern "C" void _BoxFactoryExport dllStopFactory(Fusion::Core::Game* game)
{
	game->getObjectManager()->unregisterObjectFactory(boxFactory);
    delete boxFactory;
}