#include "StableHeaders.h"
#include "OpenALSoundManager.h"

using namespace Fusion::Core;

String SoundManager::SOUND_RESOURCE_GROUP_NAME = "Sounds";

OpenALSoundManager::OpenALSoundManager(const Camera* listener) : mListener(listener)
{
	if (!alutInit(NULL, NULL))
	{
		throw new Ogre::Exception(0, "OpenAL failed to initialize", "OpenALSoundManager");
	}

	const char *mimeTypes = alutGetMIMETypes(ALUT_LOADER_BUFFER);
	Ogre::LogManager::getSingleton().logMessage("OpenALSoundManager initialized successfully");
	Ogre::LogManager::getSingleton().logMessage("OpenALSoundManager supported mime-types: " + Ogre::String(mimeTypes));

    alGetError();

	mResourceType = "Sound";

    // low, because it will likely reference other resources
    mLoadOrder = 30.0f;

    // this is how we register the ResourceManager with OGRE
    Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);

//	Ogre::ResourceGroupManager::getSingleton().createResourceGroup(SOUND_RESOURCE_GROUP_NAME);
}

OpenALSoundManager::~OpenALSoundManager(void)
{
	HashMap<String, OpenALSoundSource*>::iterator iter2 = mSoundSources.begin();
	while (iter2 != mSoundSources.end())
	{
		delete iter2->second;
		mSoundSources.erase(iter2++);
	}

	alutExit();

	// and this is how we unregister it
    Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);

	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(SOUND_RESOURCE_GROUP_NAME);
}

bool OpenALSoundManager::frameStarted(const Ogre::FrameEvent& evt)
{		
	if (mListener != NULL)
	{
		alListenerfv(AL_POSITION,    mListener->getPosition().ptr());		
		alListenerfv(AL_ORIENTATION, mListener->getOrientation().ptr());
	}

	return true;
}

bool OpenALSoundManager::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}

SoundSource* OpenALSoundManager::createSoundSource(const String& name, 
												   SoundBuffer* soundBuffer,
												   const Ogre::Vector3& position, 
											       const Ogre::Vector3& velocity)
{
	if (soundBuffer == NULL)
	{
		return NULL;
	}

	OpenALSoundSource* result = new OpenALSoundSource(name, soundBuffer, position, velocity);

	if (mSoundSources[name] != NULL)
	{
		destroySoundSource(name);
	}

	mSoundSources[name] = result;

	return result;
}

void OpenALSoundManager::destroySoundSource(const String& name)
{
	OpenALSoundSource* source = mSoundSources[name];
	if (source != NULL)
	{
		delete source;
		mSoundSources.erase(name);
	}
}

Ogre::Resource* OpenALSoundManager::createImpl(const Ogre::String &name, 
											   Ogre::ResourceHandle handle, 
											   const Ogre::String &group, 
											   bool isManual, 
											   Ogre::ManualResourceLoader *loader, 
											   const Ogre::NameValuePairList *createParams)
{
	// Get variable-length params
	Ogre::String filename;
	Ogre::NameValuePairList::const_iterator opt;
	Ogre::NameValuePairList::const_iterator end = createParams->end();

	if ((opt = createParams->find("filename")) != end)
	{
		filename = opt->second;
	}
	else 
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, 
			"createParams['filename'] == NULL", "OpenALSoundManager::createImpl");
	}

	OpenALSoundBuffer* buffer = new OpenALSoundBuffer(this, name, handle, group, filename, isManual, loader);
	//buffer->loadImpl();	
	return buffer;
}