#pragma once

#include "Prerequisites.h"

namespace Fusion
{
	namespace Core
	{
		class InputManager 
			: public Ogre::Singleton<InputManager>, public Ogre::FrameListener, 
			public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener 
		{
		public:
			
			InputManager(Ogre::RenderWindow* win);
			virtual ~InputManager( void );

			static InputManager& getSingleton(void);
			static InputManager* getSingletonPtr(void);

			virtual bool frameStarted(const Ogre::FrameEvent& evt);
			virtual bool frameEnded(const Ogre::FrameEvent& evt);

			void addKeyListener( OIS::KeyListener *keyListener, const String& instanceName );
			void addMouseListener( OIS::MouseListener *mouseListener, const String& instanceName );
			void addJoystickListener( OIS::JoyStickListener *joystickListener, const String& instanceName );

			void removeKeyListener( const String& instanceName );
			void removeMouseListener( const String& instanceName );
			void removeJoystickListener( const String& instanceName );

			void removeKeyListener( OIS::KeyListener *keyListener );
			void removeMouseListener( OIS::MouseListener *mouseListener );
			void removeJoystickListener( OIS::JoyStickListener *joystickListener );

			void removeAllListeners( void );
			void removeAllKeyListeners( void );
			void removeAllMouseListeners( void );
			void removeAllJoystickListeners( void );

			void setWindowExtents( int width, int height );

			OIS::Mouse*    getMouse( void );
			OIS::Keyboard* getKeyboard( void );
			OIS::JoyStick* getJoystick( unsigned int index );

			int getNumOfJoysticks( void );

		private:

			bool keyPressed( const OIS::KeyEvent &e );
			bool keyReleased( const OIS::KeyEvent &e );

			bool mouseMoved( const OIS::MouseEvent &e );
			bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
			bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

			bool povMoved( const OIS::JoyStickEvent &e, int pov );
			bool axisMoved( const OIS::JoyStickEvent &e, int axis );
			bool sliderMoved( const OIS::JoyStickEvent &e, int sliderID );
			bool buttonPressed( const OIS::JoyStickEvent &e, int button );
			bool buttonReleased( const OIS::JoyStickEvent &e, int button );

			OIS::InputManager *mInputSystem;
			OIS::Mouse        *mMouse;
			OIS::Keyboard     *mKeyboard;

			std::vector<OIS::JoyStick*> mJoysticks;
			std::vector<OIS::JoyStick*>::iterator itJoystick;
			std::vector<OIS::JoyStick*>::iterator itJoystickEnd;

			std::map<String, OIS::KeyListener*> mKeyListeners;
			std::map<String, OIS::MouseListener*> mMouseListeners;
			std::map<String, OIS::JoyStickListener*> mJoystickListeners;

			std::map<String, OIS::KeyListener*>::iterator itKeyListener;
			std::map<String, OIS::MouseListener*>::iterator itMouseListener;
			std::map<String, OIS::JoyStickListener*>::iterator itJoystickListener;

			std::map<String, OIS::KeyListener*>::iterator itKeyListenerEnd;
			std::map<String, OIS::MouseListener*>::iterator itMouseListenerEnd;
			std::map<String, OIS::JoyStickListener*>::iterator itJoystickListenerEnd;
		};
	}
}
