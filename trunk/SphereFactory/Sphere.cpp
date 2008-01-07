#include "Sphere.h"

using namespace Fusion::Core;
using namespace Fusion::Factories;

Sphere::Sphere(const String& name, const String& mesh, Ogre::Real mass,
			Ogre::Real radius, Game* game, World *world, OgreOde::Space* space) 
: Object(game, world, space, name)
{
	mGeometry = new OgreOde::SphereGeometry(radius, mWorld, mSpace); 	
	mGeometry->setBody(mBody);
	mGeometry->setUserObject(this);

	OgreOde::SphereMass sphereMass(mass, radius);
	mBody->setMass(sphereMass);

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

Sphere::~Sphere(void)
{
}

