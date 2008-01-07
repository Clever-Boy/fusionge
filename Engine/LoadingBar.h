#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class LoadingBar : public Ogre::ResourceGroupListener
		{
		public:
			LoadingBar();
			virtual ~LoadingBar();

			/** Show the loading bar and start listening.
			@param window The window to update 
			@param numGroupsInit The number of groups you're going to be initialising
			@param numGroupsLoad The number of groups you're going to be loading
			@param initProportion The proportion of the progress which will be taken
				up by initialisation (ie script parsing etc). Defaults to 0.7 since
				script parsing can often take the majority of the time.
			*/
			virtual void start(Ogre::RenderWindow* window, unsigned short numGroupsInit = 1, 
				unsigned short numGroupsLoad = 1, Ogre::Real initProportion = 0.70f);

			/** Hide the loading bar and stop listening. 
			*/
			virtual void finish(void);

			// ResourceGroupListener callbacks
			void resourceGroupScriptingStarted(const String& groupName, size_t scriptCount);

			void scriptParseStarted(const String& scriptName);

			void scriptParseEnded(const String& scriptName);

			void resourceGroupScriptingEnded(const String& groupName);

			void resourceGroupLoadStarted(const String& groupName, size_t resourceCount);
			
			void resourceLoadStarted(const Ogre::ResourcePtr& resource);

			void resourceLoadEnded(void);

			void worldGeometryStageStarted(const String& description);
			
			void worldGeometryStageEnded(void);
			
			void resourceGroupLoadEnded(const String& groupName);

		protected:
			Ogre::RenderWindow* mWindow;
			Ogre::Overlay* mLoadOverlay;
			Ogre::Real mInitProportion;
			unsigned short mNumGroupsInit;
			unsigned short mNumGroupsLoad;
			Ogre::Real mProgressBarMaxSize;
			Ogre::Real mProgressBarScriptSize;
			Ogre::Real mProgressBarInc;
			Ogre::OverlayElement* mLoadingBarElement;
			Ogre::OverlayElement* mLoadingDescriptionElement;
			Ogre::OverlayElement* mLoadingCommentElement;
		};
	}
}