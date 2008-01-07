#include "StableHeaders.h"
#include "SoundSource.h"

using namespace Fusion::Core;

SoundSource::SoundSource(const String& name, SoundBuffer* buffer) : mName(name), mSoundBuffer(buffer)
{
}

SoundSource::~SoundSource(void)
{
}

SoundBuffer* SoundSource::getSoundBuffer(void)
{
	return this->mSoundBuffer;
}

#define topSoundSource(L)	((SoundSource **)lua_touserdata(L, 1))

static SoundSource* toSoundSource(lua_State *L) 
{
	SoundSource **obj = topSoundSource(L);
	if (*obj == NULL)
		luaL_error(L, "attempt to use an invalid SoundSource");

	return *obj;
}

int SoundSource::_play(lua_State *L)
{
	SoundSource* sound = toSoundSource(L);
	sound->play();
	return 0;
}

int SoundSource::_pause(lua_State *L)
{
	SoundSource* sound = toSoundSource(L);
	sound->pause();
	return 0;
}

int SoundSource::_stop(lua_State *L)
{
	SoundSource* sound = toSoundSource(L);
	sound->stop();
	return 0;
}

int SoundSource::_state(lua_State *L)
{
	SoundSource* sound = toSoundSource(L);

	sound->getState();

	return 1;
}

int SoundSource::_gain(lua_State *L)
{
	SoundSource* sound = toSoundSource(L);

	if (lua_isnoneornil(L, 2))
	{
		float value = sound->getGain();
		lua_pushnumber(L, value);
		return 1;
	} 
	else 
	{		
		double value = luaL_checknumber(L, 2);
		sound->setGain((float) value);
		return 0;
	}
}

int SoundSource::_pitch(lua_State *L)
{
	SoundSource* sound = toSoundSource(L);

	if (lua_isnoneornil(L, 2))
	{
		float value = sound->getPitch();
		lua_pushnumber(L, value);
		return 1;
	} 
	else 
	{		
		double value = luaL_checknumber(L, 2);
		sound->setPitch((float) value);
		return 0;
	}
}

int SoundSource::_tostring(lua_State *L)
{
	char buffer[128];

	SoundSource* sound = toSoundSource(L);	

	sprintf(buffer, "SoundSource - %s | GAIN = %.2f | PITCH = %.2f", 
		sound->getName().c_str(), sound->getGain(), sound->getPitch());

	lua_pushstring(L, buffer);

	return 1;
}

const luaL_Reg SoundSource::methods[] = {
	{"play", &SoundSource::_play},
	{"pause", &SoundSource::_pause},
	{"stop", &SoundSource::_stop},
	{"state", &SoundSource::_state},
	{"gain", &SoundSource::_gain},
	{"pitch", &SoundSource::_pitch},
	{"__tostring", &SoundSource::_tostring},	
	{NULL, NULL}
};

int SoundSource::openLuaLib(lua_State *L)
{
	luaL_newmetatable(L, LUA_SOUNDSOURCEHANDLE);  /* create metatable for object handles */
	lua_pushvalue(L, -1);  /* push metatable */
	lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
	luaL_register(L, NULL, methods);  /* object methods */

	return 1;
}