#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class OpenALSoundBuffer : public SoundBuffer
		{
		public:
			OpenALSoundBuffer(Ogre::ResourceManager *creator, const String &name, 
			Ogre::ResourceHandle handle, const String &group, const String& fileName, 
			bool isManual = false, Ogre::ManualResourceLoader *loader = 0);
			
			~OpenALSoundBuffer(void);

			virtual void* getData(void);
			    
			// must implement these from the Ogre::Resource interface
			virtual void loadImpl();
			virtual void unloadImpl();
			virtual size_t calculateSize() const;

		private:
			String mFileName;
			ALuint mBuffer;
		};
	}
}
