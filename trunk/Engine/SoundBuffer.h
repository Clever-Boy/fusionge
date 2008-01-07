#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class SoundBuffer : public Ogre::Resource
		{
		public:
			SoundBuffer(Ogre::ResourceManager *creator, const Ogre::String &name, 
			Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false, 
			Ogre::ManualResourceLoader *loader = 0);

			virtual void* getData(void) = 0;
		};

		class SoundBufferPtr : public Ogre::SharedPtr<SoundBuffer> 
		{
		public:
			SoundBufferPtr() : Ogre::SharedPtr<SoundBuffer>() {}
			explicit SoundBufferPtr(SoundBuffer *rep) : Ogre::SharedPtr<SoundBuffer>(rep) {}
			SoundBufferPtr(const SoundBufferPtr &r) : Ogre::SharedPtr<SoundBuffer>(r) {} 
			SoundBufferPtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<SoundBuffer>()
			{
				// lock & copy other mutex pointer
				OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
					OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
					pRep = static_cast<SoundBuffer*>(r.getPointer());
				pUseCount = r.useCountPointer();
				if (pUseCount)
				{
					++(*pUseCount);
				}
			}

			/// Operator used to convert a ResourcePtr to a TextFilePtr
			SoundBufferPtr& operator=(const Ogre::ResourcePtr& r)
			{
				if (pRep == static_cast<SoundBuffer*>(r.getPointer()))
					return *this;
				release();
				// lock & copy other mutex pointer
				OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
					OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
					pRep = static_cast<SoundBuffer*>(r.getPointer());
				pUseCount = r.useCountPointer();
				if (pUseCount)
				{
					++(*pUseCount);
				}
				return *this;
			}

			inline SoundBuffer* get() const { return pRep; }
		};
	}
}
