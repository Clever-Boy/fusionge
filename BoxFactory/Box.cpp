#include "Box.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

Box::Box(const Ogre::String& name, const Ogre::String& mesh, Ogre::Real mass,
			const Ogre::Vector3& size, Game* game, World *world, OgreOde::Space* space) 
: Object(game, world, space, name) 
{ 	
	mGeometry = new OgreOde::BoxGeometry(size, mWorld, mSpace); 	
	mGeometry->setBody(mBody);
	mGeometry->setUserObject(this);

	OgreOde::BoxMass boxMass(mass, size);
	mBody->setMass(boxMass);

	mEntity = mWorld->getSceneManager()->createEntity(name + "_Entity", mesh);
    mEntity->setQueryFlags(GEOMETRY_QUERY_MASK);
	mEntity->setNormaliseNormals(true);
	mEntity->setCastShadows(true);
	mEntity->setUserObject(mGeometry);

	mNode = mWorld->getSceneManager()->getRootSceneNode()->createChildSceneNode(name + "_Node");
	mNode->attachObject(mEntity);
	mNode->attachObject(mBody);
	mNode->setScale(size * 0.1);
}

Box::~Box(void) 
{
	if (mNode)
	{
		mWorld->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(mNode->getName());
	}
}
