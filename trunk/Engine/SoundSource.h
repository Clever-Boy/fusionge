#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		#define LUA_SOUNDSOURCEHANDLE  "Fusion::Core::SoundSource*"

		class SoundSource
		{
		public:
			enum State {
				Playing,
				Paused,
				Stopped
			};

			SoundSource(const String& name, SoundBuffer* sound);
			virtual ~SoundSource(void);

			const String& getName() { return mName; }

			virtual SoundBuffer* getSoundBuffer(void);

			virtual Ogre::Vector3 getPosition(void) = 0;
			virtual void setPosition(const Ogre::Vector3& position) = 0;

			virtual Ogre::Vector3 getVelocity(void) = 0;
			virtual void setVelocity(const Ogre::Vector3& velocity) = 0;

			virtual void setPitch(float pitch) = 0;
			virtual float getPitch(void) = 0;

			virtual void setGain(float gain) = 0;
			virtual float getGain(void) = 0;

			virtual void setLoop(bool loop) = 0;
			virtual bool isLoop(void) = 0;

			virtual void play(void) = 0;
			virtual void pause(void) = 0;
			virtual void stop(void) = 0;

			virtual SoundSource::State getState(void) = 0;

			static int openLuaLib(lua_State *L);

			static int _play(lua_State *L);
			static int _pause(lua_State *L);
			static int _stop(lua_State *L);
			static int _state(lua_State *L);
			static int _gain(lua_State *L);
			static int _pitch(lua_State *L);
			static int _tostring(lua_State *L);		

		protected:
			static const luaL_Reg methods[];

		protected:
			SoundBuffer* mSoundBuffer;
			String mName;
		};
	}
}