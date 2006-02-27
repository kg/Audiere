// Based on code written for Serotek Corporation (http://www.freedombox.info/)

#include <audiere.h>
#include <assert.h>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include "luaAudiere.h"
}

#include "luaAudiereTypes.h"
#include "internal.h"

using namespace audiere;

LUA_AUDIERE_API void
luaAudiere_pushAudioDevice(lua_State* L, AudioDevice* obj) {
  pushRefCounted(L, obj, MT_AudioDevice);
}

LUA_AUDIERE_API bool
luaAudiere_isAudioDevice(lua_State* L, int idx) {
  return isRefCounted(L, idx, MT_AudioDevice);
}

LUA_AUDIERE_API AudioDevice*
luaAudiere_toAudioDevice(lua_State* L, int idx) {
  return static_cast<AudioDevice*>(toRefCounted(L, idx, MT_AudioDevice));
}

LUA_AUDIERE_API AudioDevice*
luaAudiere_checkAudioDevice(lua_State* L, int arg) {
  return static_cast<AudioDevice*>(checkRefCounted(L, arg, MT_AudioDevice));
}

static int wrap_update(lua_State* L) {
  lua_settop(L, 1);
  AudioDevice* self = luaAudiere_checkAudioDevice(L, 1);
  self->update();
  return 0;
}

static int wrap_openStream(lua_State* L) {
  lua_settop(L, 2);
  AudioDevice* self = luaAudiere_checkAudioDevice(L, 1);
  SampleSource* source = luaAudiere_checkSampleSource(L, 2);
  luaAudiere_pushOutputStream(L, self->openStream(source));
  return 1;
}

static int wrap_getName(lua_State* L) {
  lua_settop(L, 1);
  AudioDevice* self = luaAudiere_checkAudioDevice(L, 1);
  lua_pushstring(L, self->getName());
  return 1;
}

static const luaL_Reg AudioDevice_m[] = {
  {"update", wrap_update},
  {"openStream", wrap_openStream},
  {"getName", wrap_getName},
  {0, 0}
};

LUA_AUDIERE_API void
luaAudiere_registerAudioDevice(lua_State* L) {
  registerRefCounted(L);
  luaAudiere_registerSampleSource(L);
  luaAudiere_registerOutputStream(L);
  registerInterface(L, MT_AudioDevice, MT_RefCounted, AudioDevice_m,
                    "AudioDevice");
}
