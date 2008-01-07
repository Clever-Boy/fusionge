#pragma once

#include "Prerequisites.h"
#include "OpenALSoundBuffer.h"
#include "OpenALSoundSource.h"

namespace Fusion
{
	namespace Core
	{
		class OpenALSoundManager : public SoundManager
		{
		public:
			OpenALSoundManager(const Camera* listener);
			virtual ~OpenALSoundManager(void);

			virtual bool frameStarted(const Ogre::FrameEvent& evt);
			virtual bool frameEnded(const Ogre::FrameEvent& evt);

			virtual SoundSource* createSoundSource(const String& name, 
												   SoundBuffer* buffer,
												   const Ogre::Vector3& position, 
												   const Ogre::Vector3& velocity = Ogre::Vector3::ZERO);

			virtual void destroySoundSource(const String& name);

			// must implement this from ResourceManager's interface
			Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle, 
				const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
				const Ogre::NameValuePairList *createParams);

		protected:
			const Camera* mListener;
			HashMap<String, OpenALSoundSource*> mSoundSources;
		};
	}
}
