#include "StableHeaders.h"
#include "SoundBuffer.h"

using namespace Fusion::Core;

SoundBuffer::SoundBuffer(Ogre::ResourceManager *creator, const Ogre::String &name, 
        Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual, 
		Ogre::ManualResourceLoader *loader) 

		: Ogre::Resource(creator, name, handle, group, isManual, loader)
{
	createParamDictionary("SoundBuffer");
}