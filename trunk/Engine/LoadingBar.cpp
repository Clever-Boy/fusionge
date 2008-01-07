#include "StableHeaders.h"
#include "LoadingBar.h"

using namespace Fusion::Core;

LoadingBar::LoadingBar(void)
{
}

LoadingBar::~LoadingBar(void)
{
}

void LoadingBar::start(Ogre::RenderWindow* window, unsigned short numGroupsInit, 
					   unsigned short numGroupsLoad, Ogre::Real initProportion)
{
	mWindow = window;
	mNumGroupsInit = numGroupsInit;
	mNumGroupsLoad = numGroupsLoad;
	mInitProportion = initProportion;
	// We need to pre-initialise the 'Bootstrap' group so we can use
	// the basic contents in the loading screen
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");

	Ogre::OverlayManager& omgr = Ogre::OverlayManager::getSingleton();
	mLoadOverlay = (Ogre::Overlay*)omgr.getByName("Core/LoadOverlay");
	if (!mLoadOverlay)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, 
			"Cannot find loading overlay", "ExampleLoadingBar::start");
	}
	mLoadOverlay->show();

	// Save links to the bar and to the loading text, for updates as we go
	mLoadingBarElement = omgr.getOverlayElement("Core/LoadPanel/Bar/Progress");
	mLoadingCommentElement = omgr.getOverlayElement("Core/LoadPanel/Comment");
	mLoadingDescriptionElement = omgr.getOverlayElement("Core/LoadPanel/Description");

	Ogre::OverlayElement* barContainer = omgr.getOverlayElement("Core/LoadPanel/Bar");
	mProgressBarMaxSize = barContainer->getWidth();
	mLoadingBarElement->setWidth(0);

	// self is listener
	Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(this);
}

void LoadingBar::finish(void)
{
	// hide loading screen
	mLoadOverlay->hide();

	// Unregister listener
	Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener(this);

}

// ResourceGroupListener callbacks
void LoadingBar::resourceGroupScriptingStarted(const String& groupName, size_t scriptCount)
{
	assert(mNumGroupsInit > 0 && "You stated you were not going to init "
		"any groups, but you did! Divide by zero would follow...");
	// Lets assume script loading is 70%
	mProgressBarInc = mProgressBarMaxSize * mInitProportion / (Ogre::Real)scriptCount;
	mProgressBarInc /= mNumGroupsInit;
	mLoadingDescriptionElement->setCaption("Parsing scripts...");
	mWindow->update();
}

void LoadingBar::scriptParseStarted(const String& scriptName)
{
	mLoadingCommentElement->setCaption(scriptName);
	mWindow->update();
}

void LoadingBar::scriptParseEnded(const String& scriptName)
{
	mLoadingBarElement->setWidth(
		mLoadingBarElement->getWidth() + mProgressBarInc);
	mWindow->update();
}

void LoadingBar::resourceGroupScriptingEnded(const String& groupName) { }

void LoadingBar::resourceGroupLoadStarted(const String& groupName, size_t resourceCount)
{
	assert(mNumGroupsLoad > 0 && "You stated you were not going to load "
		"any groups, but you did! Divide by zero would follow...");
	mProgressBarInc = mProgressBarMaxSize * (1-mInitProportion) / 
		(Ogre::Real)resourceCount;
	mProgressBarInc /= mNumGroupsLoad;
	mLoadingDescriptionElement->setCaption("Loading resources...");
	mWindow->update();
}

void LoadingBar::resourceLoadStarted(const Ogre::ResourcePtr& resource)
{
	mLoadingCommentElement->setCaption(resource->getName());
	mWindow->update();
}

void LoadingBar::resourceLoadEnded(void) { }

void LoadingBar::worldGeometryStageStarted(const String& description)
{
	mLoadingCommentElement->setCaption(description);
	mWindow->update();
}

void LoadingBar::worldGeometryStageEnded(void)
{
	mLoadingBarElement->setWidth(mLoadingBarElement->getWidth() + mProgressBarInc);
	mWindow->update();
}

void LoadingBar::resourceGroupLoadEnded(const String& groupName) { }