// Based on code written for Serotek Corporation (http://www.freedombox.info/)

#ifndef internal_h
#define internal_h

#include <audiere.h>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include "luaAudiere.h"
}

#include "luaAudiereTypes.h"

// Interface metatable names
#define MT_RefCounted "audiere::RefCounted"
#define MT_File "audiere::File"
#define MT_SampleSource "audiere::SampleSource"
#define MT_LoopPointSource "audiere::LoopPointSource"
#define MT_OutputStream "audiere::OutputStream"
#define MT_AudioDevice "audiere::AudioDevice"

// Generic object support
void getMetatableByName(lua_State* L, const char* mtName);
void setMetatable(lua_State* L, const char* mtName);
bool newObjectType(lua_State* L, const char* mtName, const char* baseMTName,
                   const luaL_Reg* methods, const char* visibleName = 0);
bool isObject(lua_State* L, int idx, const char* mtName);
void* toObject(lua_State* L, int idx, const char* mtName);
void* checkObject(lua_State* L, int arg, const char* mtName);
void initCacheTable(lua_State* L);
bool isPointerCached(lua_State* L, void* p);
void newCachedPointer(lua_State* L, void* p, const char* mtName);

// Audiere interface support
void registerInterface(lua_State* L, const char* mtName,
                       const char* baseMTName, const luaL_Reg* methods,
                       const char* visibleName);
void pushRefCounted(lua_State* L, audiere::RefCounted* obj,
                    const char* mtName);
bool isRefCounted(lua_State* L, int idx, const char* mtName);
audiere::RefCounted* toRefCounted(lua_State* L, int idx, const char* mtName);
audiere::RefCounted* checkRefCounted(lua_State* L, int arg,
                                     const char* mtName);
void registerRefCounted(lua_State* L);

#endif
