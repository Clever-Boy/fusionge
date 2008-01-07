#include "StableHeaders.h"
#include "ObjectFactory.h"
#include "ObjectManager.h"

using namespace Fusion::Core;

ObjectFactory::ObjectFactory(const String& _name, Game* game) 
: name(_name), mGame(game), mSceneManager(game->getSceneManager())
{
	this->mWorld = game->getObjectManager()->getWorld();
	this->mSpace = game->getObjectManager()->getSpace();
}

ObjectFactory::~ObjectFactory(void) {}

const String& ObjectFactory::getName(void)
{
	return this->name;
}
