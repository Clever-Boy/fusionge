#include "StableHeaders.h"
#include "OpenALSoundBuffer.h"

using namespace Fusion::Core;

OpenALSoundBuffer::OpenALSoundBuffer(Ogre::ResourceManager *creator, const String &name, 
        Ogre::ResourceHandle handle, const String &group, const String& fileName, 
		bool isManual, Ogre::ManualResourceLoader *loader) : mFileName(fileName)
, SoundBuffer(creator, name, handle, group, isManual, loader)
{
	
}

OpenALSoundBuffer::~OpenALSoundBuffer(void)
{
}

void* OpenALSoundBuffer::getData(void)
{
	return &mBuffer;
}

void OpenALSoundBuffer::loadImpl()
{
	//Ogre::DataStreamPtr ds = Ogre::ResourceGroupManager::getSingleton().openResource(name, SoundManager::SOUND_RESOURCE_GROUP_NAME, false, this); 
	

	mBuffer = alutCreateBufferFromFile(mFileName.c_str());
	if (mBuffer == AL_NONE)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, 
			"Cannot find loading sound", "OpenALSoundBuffer::loadImpl");
	}
}

void OpenALSoundBuffer::unloadImpl()
{
	alDeleteBuffers(1, &mBuffer);
}

size_t OpenALSoundBuffer::calculateSize() const
{
	ALint value;
	alGetBufferi(mBuffer, AL_SIZE, &value);
	return (size_t) value;
}