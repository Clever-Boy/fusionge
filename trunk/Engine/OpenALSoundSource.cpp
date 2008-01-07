#include "StableHeaders.h"
#include "OpenALSoundSource.h"

using namespace Fusion::Core;

OpenALSoundSource::OpenALSoundSource(const String& name, SoundBuffer* sound, const Ogre::Vector3& position, 
									 const Ogre::Vector3& velocity) : SoundSource(name, sound)
{
	alGenSources(1, &(this->mSource));
	if (alGetError() != AL_NO_ERROR)
	{
		throw new Ogre::Exception(0, "Unable to create SoundSource", "OpenALSoundSource");
	}

	ALint* buffer = (ALint*)mSoundBuffer->getData();

	alSourcei (this->mSource, AL_BUFFER,   *buffer  );
    alSourcef (this->mSource, AL_PITCH,    1.0f     );
    alSourcef (this->mSource, AL_GAIN,     1.0f     );
	alSourcefv(this->mSource, AL_POSITION, position.ptr());
	alSourcefv(this->mSource, AL_VELOCITY, velocity.ptr());    
}

OpenALSoundSource::~OpenALSoundSource(void)
{
	alDeleteSources(1, &(this->mSource));
}

Ogre::Vector3 OpenALSoundSource::getPosition(void)
{
	Ogre::Vector3 result;

	alGetSourcef(this->mSource, AL_POSITION, result.ptr());

	return result;
}

void OpenALSoundSource::setPosition(const Ogre::Vector3& position)
{
	alSourcefv(this->mSource, AL_POSITION, position.ptr());
}

Ogre::Vector3 OpenALSoundSource::getVelocity(void)
{
	Ogre::Vector3 result;

	alGetSourcef(this->mSource, AL_VELOCITY, result.ptr());

	return result;
}

void OpenALSoundSource::setVelocity(const Ogre::Vector3& velocity)
{
	alSourcefv(this->mSource, AL_VELOCITY, velocity.ptr()); 
}

void OpenALSoundSource::setPitch(float pitch)
{
	alSourcef(this->mSource, AL_PITCH, pitch);
}

float OpenALSoundSource::getPitch(void)
{
	float result = -1;

	alGetSourcef(this->mSource, AL_PITCH, &result);

	return result;
}

void OpenALSoundSource::setGain(float gain)
{
	alSourcef(this->mSource, AL_GAIN, gain);
}

float OpenALSoundSource::getGain(void)
{
	float result = -1;

	alGetSourcef(this->mSource, AL_GAIN, &result);

	return result;
}

void OpenALSoundSource::setLoop(bool loop)
{
	alSourcei(this->mSource, AL_LOOPING, loop);
}

bool OpenALSoundSource::isLoop(void)
{
	int result = 0;

	alGetSourcei(this->mSource, AL_LOOPING, &result);

	return result != 0;
}

void OpenALSoundSource::play(void)
{
	alSourcePlay(this->mSource);
}

void OpenALSoundSource::pause(void)
{
	alSourcePause(this->mSource);
}

void OpenALSoundSource::stop(void)
{
	alSourceStop(this->mSource);
}

SoundSource::State OpenALSoundSource::getState(void)
{
	ALint state;
	alGetSourcei(this->mSource, AL_SOURCE_STATE, &state);

	switch (state)
	{
	case AL_PLAYING:
		return SoundSource::Playing;
	case AL_PAUSED:
		return SoundSource::Paused;
	default:
		return SoundSource::Stopped;
	}
}