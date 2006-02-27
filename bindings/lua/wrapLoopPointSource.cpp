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
luaAudiere_pushLoopPointSource(lua_State* L, LoopPointSource* obj) {
  pushRefCounted(L, obj, MT_LoopPointSource);
}

LUA_AUDIERE_API bool
luaAudiere_isLoopPointSource(lua_State* L, int idx) {
  return isRefCounted(L, idx, MT_LoopPointSource);
}

LUA_AUDIERE_API LoopPointSource*
luaAudiere_toLoopPointSource(lua_State* L, int idx) {
  return
    static_cast<LoopPointSource*>(toRefCounted(L, idx, MT_LoopPointSource));
}

LUA_AUDIERE_API LoopPointSource*
luaAudiere_checkLoopPointSource(lua_State* L, int arg) {
  return
    static_cast<LoopPointSource*>(checkRefCounted(L, arg, MT_LoopPointSource));
}

static int wrap_addLoopPoint(lua_State* L) {
  lua_settop(L, 4);
  LoopPointSource* self = luaAudiere_checkLoopPointSource(L, 1);
  int location = luaL_checkint(L, 2);
  int target = luaL_checkint(L, 3);
  int loopCount = luaL_checkint(L, 4);
  self->addLoopPoint(location, target, loopCount);
  return 0;
}

static int checkLoopPointIndex(lua_State* L, LoopPointSource* self, int arg) {
  int index = luaL_checkint(L, arg);
  luaL_argcheck(L, index >= 0, arg, "non-negative integer expected");
  luaL_argcheck(L, index < self->getLoopPointCount(), arg,
                "index out of range");
  return index;
}

static int wrap_removeLoopPoint(lua_State* L) {
  lua_settop(L, 2);
  LoopPointSource* self = luaAudiere_checkLoopPointSource(L, 1);
  int index = checkLoopPointIndex(L, self, 2);
  self->removeLoopPoint(index);
  return 0;
}

static int wrap_getLoopPointCount(lua_State* L) {
  lua_settop(L, 1);
  LoopPointSource* self = luaAudiere_checkLoopPointSource(L, 1);
  lua_pushinteger(L, self->getLoopPointCount());
  return 1;
}

static int wrap_getLoopPoint(lua_State* L) {
  lua_settop(L, 2);
  LoopPointSource* self = luaAudiere_checkLoopPointSource(L, 1);
  int index = checkLoopPointIndex(L, self, 2);
  int location;
  int target;
  int loopPointCount;
  if (self->getLoopPoint(index, location, target, loopPointCount)) {
    lua_pushinteger(L, location);
    lua_pushinteger(L, target);
    lua_pushinteger(L, loopPointCount);
    return 3;
  } else
    return 0;
}

static const luaL_Reg LoopPointSource_m[] = {
  {"addLoopPoint", wrap_addLoopPoint},
  {"removeLoopPoint", wrap_removeLoopPoint},
  {"getLoopPointCount", wrap_getLoopPointCount},
  {"getLoopPoint", wrap_getLoopPoint},
  {0, 0}
};

LUA_AUDIERE_API void
luaAudiere_registerLoopPointSource(lua_State* L) {
  luaAudiere_registerSampleSource(L);
  registerInterface(L, MT_LoopPointSource, MT_SampleSource, LoopPointSource_m,
                    "LoopPointSource");
}
