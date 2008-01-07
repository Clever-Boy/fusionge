
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Fusion.h>

#ifdef _WINDOWS
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char**argv)
#endif
{	
	Fusion::Core::Game* game = new Fusion::Core::Game();

	game->start("lua\\game.lua");

    return 0;
}
