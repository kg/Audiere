#ifndef luaAudiereTypes_h
#define luaAudiereTypes_h

#include <audiere.h>

extern "C" {

#include <lua.h>
#include "luaAudiere.h"

// File
LUA_AUDIERE_API void
luaAudiere_pushFile(lua_State* L, audiere::File* obj);
LUA_AUDIERE_API bool
luaAudiere_isFile(lua_State* L, int idx);
LUA_AUDIERE_API audiere::File*
luaAudiere_toFile(lua_State* L, int idx);
LUA_AUDIERE_API audiere::File*
luaAudiere_checkFile(lua_State* L, int arg);
LUA_AUDIERE_API void
luaAudiere_registerFile(lua_State* L);

// SampleSource
LUA_AUDIERE_API void
luaAudiere_pushSampleSource(lua_State* L, audiere::SampleSource* obj);
LUA_AUDIERE_API bool
luaAudiere_isSampleSource(lua_State* L, int idx);
LUA_AUDIERE_API audiere::SampleSource*
luaAudiere_toSampleSource(lua_State* L, int idx);
LUA_AUDIERE_API audiere::SampleSource*
luaAudiere_checkSampleSource(lua_State* L, int arg);
LUA_AUDIERE_API void
luaAudiere_registerSampleSource(lua_State* L);

// LoopPointSource
LUA_AUDIERE_API void
luaAudiere_pushLoopPointSource(lua_State* L, audiere::LoopPointSource* obj);
LUA_AUDIERE_API bool
luaAudiere_isLoopPointSource(lua_State* L, int idx);
LUA_AUDIERE_API audiere::LoopPointSource*
luaAudiere_toLoopPointSource(lua_State* L, int idx);
LUA_AUDIERE_API audiere::LoopPointSource*
luaAudiere_checkLoopPointSource(lua_State* L, int arg);
LUA_AUDIERE_API void
luaAudiere_registerLoopPointSource(lua_State* L);

// OutputStream
LUA_AUDIERE_API void
luaAudiere_pushOutputStream(lua_State* L, audiere::OutputStream* obj);
LUA_AUDIERE_API bool
luaAudiere_isOutputStream(lua_State* L, int idx);
LUA_AUDIERE_API audiere::OutputStream*
luaAudiere_toOutputStream(lua_State* L, int idx);
LUA_AUDIERE_API audiere::OutputStream*
luaAudiere_checkOutputStream(lua_State* L, int arg);
LUA_AUDIERE_API void
luaAudiere_registerOutputStream(lua_State* L);

// AudioDevice
LUA_AUDIERE_API void
luaAudiere_pushAudioDevice(lua_State* L, audiere::AudioDevice* obj);
LUA_AUDIERE_API bool
luaAudiere_isAudioDevice(lua_State* L, int idx);
LUA_AUDIERE_API audiere::AudioDevice*
luaAudiere_toAudioDevice(lua_State* L, int idx);
LUA_AUDIERE_API audiere::AudioDevice*
luaAudiere_checkAudioDevice(lua_State* L, int arg);
LUA_AUDIERE_API void
luaAudiere_registerAudioDevice(lua_State* L);

}

#endif
