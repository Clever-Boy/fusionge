#include "StableHeaders.h"
#include "InputManager.h"

using namespace Fusion::Core;

template<> InputManager* Ogre::Singleton<InputManager>::ms_Singleton = 0;

InputManager* InputManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

InputManager& InputManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );
}

InputManager::InputManager( Ogre::RenderWindow *renderWindow ) 
{
	// Setup basic variables
	OIS::ParamList paramList;    
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	// Get window handle
	renderWindow->getCustomAttribute( "WINDOW", &windowHnd );	
	
	// Fill parameter list
	windowHndStr << (unsigned int) windowHnd;
	paramList.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );

	String oisMajorVersion = Ogre::StringConverter::toString((OIS::InputManager::getVersionNumber() & 0xFFFF0000) >> 16);
	String oisMinorVersion = Ogre::StringConverter::toString((OIS::InputManager::getVersionNumber() & 0x0000FF00) >> 8);
	Ogre::LogManager::getSingleton().logMessage("************************************");
	Ogre::LogManager::getSingleton().logMessage("Using OIS version " + oisMajorVersion + "." + oisMinorVersion + " (" + OIS::InputManager::getVersionName() + ")***");
	Ogre::LogManager::getSingleton().logMessage("************************************");

	// Create inputsystem
	mInputSystem = OIS::InputManager::createInputSystem( paramList );

	// If possible create a buffered keyboard
	// (note: if below line doesn't compile, try:  if (mInputSystem->getNumberOfDevices(OIS::OISKeyboard) > 0) {
	if( mInputSystem->numKeyboards() > 0 ) {
		mKeyboard = static_cast<OIS::Keyboard*>( mInputSystem->createInputObject( OIS::OISKeyboard, true ) );
		mKeyboard->setEventCallback( this );
	}

	// If possible create a buffered mouse
	// (note: if below line doesn't compile, try:  if (mInputSystem->getNumberOfDevices(OIS::OISMouse) > 0) {
	if( mInputSystem->numMice() > 0 ) {
		mMouse = static_cast<OIS::Mouse*>( mInputSystem->createInputObject( OIS::OISMouse, true ) );
		mMouse->setEventCallback( this );

		// Get window size
		unsigned int width = renderWindow->getWidth();
		unsigned int height = renderWindow->getHeight();
		
		// Set mouse region
		this->setWindowExtents( width, height );
	}

	// If possible create all joysticks in buffered mode
	// (note: if below line doesn't compile, try:  if (mInputSystem->getNumberOfDevices(OIS::OISJoystick) > 0) {
	if( mInputSystem->numJoySticks() > 0 ) {
		mJoysticks.resize( mInputSystem->numJoySticks() );

		itJoystick    = mJoysticks.begin();
		itJoystickEnd = mJoysticks.end();
		for(; itJoystick != itJoystickEnd; ++itJoystick ) {
			(*itJoystick) = static_cast<OIS::JoyStick*>( mInputSystem->createInputObject( OIS::OISJoyStick, true ) );
			(*itJoystick)->setEventCallback( this );
		}
	}    
}

InputManager::~InputManager( void ) 
{
	if( mMouse ) {
		mInputSystem->destroyInputObject( mMouse );
		mMouse = 0;
	}

	if( mKeyboard ) {
		mInputSystem->destroyInputObject( mKeyboard );
		mKeyboard = 0;
	}

	if( mJoysticks.size() > 0 ) {
		itJoystick    = mJoysticks.begin();
		itJoystickEnd = mJoysticks.end();
		for(; itJoystick != itJoystickEnd; ++itJoystick ) {
			mInputSystem->destroyInputObject( *itJoystick );
		}

		mJoysticks.clear();
	}

	OIS::InputManager::destroyInputSystem(mInputSystem);
	mInputSystem = 0;

	// Clear Listeners
	mKeyListeners.clear();
	mMouseListeners.clear();
	mJoystickListeners.clear();
}

// Override frameStarted event to process that (don't care about frameEnded)
bool InputManager::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool InputManager::frameEnded(const Ogre::FrameEvent& evt)
{
	// Need to capture / update each device every frame
    if( mMouse ) {
        mMouse->capture();
    }

    if( mKeyboard ) {
        mKeyboard->capture();
    }

    if( mJoysticks.size() > 0 ) {
        itJoystick    = mJoysticks.begin();
        itJoystickEnd = mJoysticks.end();
        for(; itJoystick != itJoystickEnd; ++itJoystick ) {
            (*itJoystick)->capture();
        }
    }

	return true;
}

void InputManager::addKeyListener( OIS::KeyListener *keyListener, const String& instanceName ) 
{
    if( mKeyboard ) {
        // Check for duplicate items
        itKeyListener = mKeyListeners.find( instanceName );
        if( itKeyListener == mKeyListeners.end() ) {
            mKeyListeners[ instanceName ] = keyListener;
        }
        else {
            // Duplicate Item
        }
    }
}

void InputManager::addMouseListener( OIS::MouseListener *mouseListener, const String& instanceName ) 
{
    if( mMouse ) {
        // Check for duplicate items
        itMouseListener = mMouseListeners.find( instanceName );
        if( itMouseListener == mMouseListeners.end() ) {
            mMouseListeners[ instanceName ] = mouseListener;
        }
        else {
            // Duplicate Item
        }
    }
}

void InputManager::addJoystickListener( OIS::JoyStickListener *joystickListener, const String& instanceName ) 
{
    if( mJoysticks.size() > 0 ) {
        // Check for duplicate items
        itJoystickListener = mJoystickListeners.find( instanceName );
        if( itJoystickListener == mJoystickListeners.end() ) {
            mJoystickListeners[ instanceName ] = joystickListener;
        }
        else {
            // Duplicate Item
        }
    }
}

void InputManager::removeKeyListener( const String& instanceName ) 
{
    // Check if item exists
    itKeyListener = mKeyListeners.find( instanceName );
    if( itKeyListener != mKeyListeners.end() ) {
        mKeyListeners.erase( itKeyListener );
    }
    else {
        // Doesn't Exist
    }
}

void InputManager::removeMouseListener( const String& instanceName ) 
{
    // Check if item exists
    itMouseListener = mMouseListeners.find( instanceName );
    if( itMouseListener != mMouseListeners.end() ) {
        mMouseListeners.erase( itMouseListener );
    }
    else {
        // Doesn't Exist
    }
}

void InputManager::removeJoystickListener( const String& instanceName ) 
{
    // Check if item exists
    itJoystickListener = mJoystickListeners.find( instanceName );
    if( itJoystickListener != mJoystickListeners.end() ) {
        mJoystickListeners.erase( itJoystickListener );
    }
    else {
        // Doesn't Exist
    }
}

void InputManager::removeKeyListener( OIS::KeyListener *keyListener ) 
{
    itKeyListener    = mKeyListeners.begin();
    itKeyListenerEnd = mKeyListeners.end();
    for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
        if( itKeyListener->second == keyListener ) {
            mKeyListeners.erase( itKeyListener );
            break;
        }
    }
}

void InputManager::removeMouseListener( OIS::MouseListener *mouseListener ) 
{
    itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
        if( itMouseListener->second == mouseListener ) {
            mMouseListeners.erase( itMouseListener );
            break;
        }
    }
}

void InputManager::removeJoystickListener( OIS::JoyStickListener *joystickListener ) 
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if( itJoystickListener->second == joystickListener ) {
            mJoystickListeners.erase( itJoystickListener );
            break;
        }
    }
}

void InputManager::removeAllListeners( void ) 
{
    mKeyListeners.clear();
    mMouseListeners.clear();
    mJoystickListeners.clear();
}

void InputManager::removeAllKeyListeners( void ) 
{
    mKeyListeners.clear();
}

void InputManager::removeAllMouseListeners( void ) 
{
    mMouseListeners.clear();
}

void InputManager::removeAllJoystickListeners( void ) 
{
    mJoystickListeners.clear();
}

void InputManager::setWindowExtents( int width, int height )
{
    // Set mouse region (if window resizes, we should alter this to reflect as well)
    const OIS::MouseState &mouseState = mMouse->getMouseState();
    mouseState.width  = width;
    mouseState.height = height;
}

OIS::Mouse* InputManager::getMouse( void ) 
{
    return mMouse;
}

OIS::Keyboard* InputManager::getKeyboard( void ) 
{
    return mKeyboard;
}

OIS::JoyStick* InputManager::getJoystick( unsigned int index ) 
{
    // Make sure it's a valid index
    if( index < mJoysticks.size() ) {
        return mJoysticks[ index ];
    }

    return 0;
}

int InputManager::getNumOfJoysticks( void )
{
    // Cast to keep compiler happy ^^
    return (int) mJoysticks.size();
}

bool InputManager::keyPressed( const OIS::KeyEvent &e ) 
{
    itKeyListener    = mKeyListeners.begin();
    itKeyListenerEnd = mKeyListeners.end();
    for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) 
	{
        if(!itKeyListener->second->keyPressed( e ))
			break;
    }

    return true;
}

bool InputManager::keyReleased( const OIS::KeyEvent &e ) 
{
    itKeyListener    = mKeyListeners.begin();
    itKeyListenerEnd = mKeyListeners.end();
    for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
        if(!itKeyListener->second->keyReleased( e ))
			break;
    }

    return true;
}

bool InputManager::mouseMoved( const OIS::MouseEvent &e ) 
{
    itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
        if(!itMouseListener->second->mouseMoved( e ))
			break;
    }

    return true;
}

bool InputManager::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
    itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
        if(!itMouseListener->second->mousePressed( e, id ))
			break;
    }

    return true;
}

bool InputManager::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
{
    itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
        if(!itMouseListener->second->mouseReleased( e, id ))
			break;
    }

    return true;
}

bool InputManager::povMoved( const OIS::JoyStickEvent &e, int pov ) 
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->povMoved( e, pov ))
			break;
    }

    return true;
}

bool InputManager::axisMoved( const OIS::JoyStickEvent &e, int axis ) 
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->axisMoved( e, axis ))
			break;
    }

    return true;
}

bool InputManager::sliderMoved( const OIS::JoyStickEvent &e, int sliderID )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->sliderMoved( e, sliderID ))
			break;
    }

    return true;
}

bool InputManager::buttonPressed( const OIS::JoyStickEvent &e, int button )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->buttonPressed( e, button ))
			break;
    }

    return true;
}

bool InputManager::buttonReleased( const OIS::JoyStickEvent &e, int button )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->buttonReleased( e, button ))
			break;
    }

    return true;
}