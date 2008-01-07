#pragma once

#include "Prerequisites.h"

#include "SoundSource.h"

namespace Fusion
{
	namespace Core
	{
		class OpenALSoundSource : public SoundSource
		{
		public:
			OpenALSoundSource(const String& name, SoundBuffer* sound, 
				const Ogre::Vector3& position = Ogre::Vector3::ZERO, 
				const Ogre::Vector3& velocity = Ogre::Vector3::ZERO);

			virtual ~OpenALSoundSource(void);

			virtual Ogre::Vector3 getPosition(void);
			virtual void setPosition(const Ogre::Vector3& position);

			virtual Ogre::Vector3 getVelocity(void);
			virtual void setVelocity(const Ogre::Vector3& velocity);

			virtual void setPitch(float pitch);
			virtual float getPitch(void);

			virtual void setGain(float gain);
			virtual float getGain(void);

			virtual void setLoop(bool loop);
			virtual bool isLoop(void);

			virtual void play(void);
			virtual void pause(void);
			virtual void stop(void);

			virtual SoundSource::State getState(void);

		protected:
			ALuint mSource;
		};
	}
}
