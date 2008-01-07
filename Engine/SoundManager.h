#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class SoundManager : public Ogre::FrameListener, public Ogre::ResourceManager
		{		
		public:
			static String SOUND_RESOURCE_GROUP_NAME;

			virtual SoundSource* createSoundSource(const String& name, 
												   SoundBuffer* soundBuffer,
												   const Ogre::Vector3& position = Ogre::Vector3::ZERO, 
												   const Ogre::Vector3& velocity = Ogre::Vector3::ZERO) = 0;

			virtual void destroySoundSource(const String& name) = 0;		

			/*virtual void setMusic(SoundBuffer* buffer) = 0;
			virtual void setMusicVolume(float volume) = 0;
			virtual void setSoundVolume(float volume) = 0;*/
		};
	}
}