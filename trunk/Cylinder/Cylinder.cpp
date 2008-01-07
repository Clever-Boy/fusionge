#include "Cylinder.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

Cylinder::Cylinder(const Ogre::String& name, const Ogre::String& mesh, Ogre::Real mass,
				   Ogre::Real radius, Ogre::Real length, const Ogre::Vector3& direction,
				   Game* game, World *world, OgreOde::Space* space)
: Object(game, world, space, name) 
{ 	
	mGeometry = new OgreOde::CylinderGeometry(radius, length, mWorld, mSpace); 	
	mGeometry->setBody(mBody);
	mGeometry->setUserObject(this);

	OgreOde::CylinderMass cylinderMass(mass, direction, radius, length);
	mBody->setMass(cylinderMass);

	mEntity = mWorld->getSceneManager()->createEntity(name + "_Entity", mesh);
    mEntity->setQueryFlags(GEOMETRY_QUERY_MASK);
	mEntity->setNormaliseNormals(true);
	mEntity->setCastShadows(true);
	mEntity->setUserObject(mGeometry);

	mNode = mWorld->getSceneManager()->getRootSceneNode()->createChildSceneNode(name + "_Node");
	mNode->attachObject(mEntity);
	mNode->attachObject(mBody);
	mNode->setScale(radius * 0.2, radius * 0.2, radius * 0.2);
}

Cylinder::~Cylinder(void) 
{
	if (mNode)
	{
		mWorld->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(mNode->getName());
	}
}