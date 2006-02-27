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
luaAudiere_pushOutputStream(lua_State* L, OutputStream* obj) {
  pushRefCounted(L, obj, MT_OutputStream);
}

LUA_AUDIERE_API bool
luaAudiere_isOutputStream(lua_State* L, int idx) {
  return isRefCounted(L, idx, MT_OutputStream);
}

LUA_AUDIERE_API OutputStream*
luaAudiere_toOutputStream(lua_State* L, int idx) {
  return static_cast<OutputStream*>(toRefCounted(L, idx, MT_OutputStream));
}

LUA_AUDIERE_API OutputStream*
luaAudiere_checkOutputStream(lua_State* L, int arg) {
  return static_cast<OutputStream*>(checkRefCounted(L, arg, MT_OutputStream));
}

static int wrap_play(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  self->play();
  return 0;
}

static int wrap_stop(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  self->stop();
  return 0;
}

static int wrap_isPlaying(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushboolean(L, self->isPlaying());
  return 1;
}

static int wrap_reset(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  self->reset();
  return 0;
}

static int wrap_setRepeat(lua_State* L) {
  lua_settop(L, 2);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  bool repeat = lua_toboolean(L, 2) != 0;
  self->setRepeat(repeat);
  return 0;
}

static int wrap_getRepeat(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushboolean(L, self->getRepeat());
  return 1;
}

static int wrap_setVolume(lua_State* L) {
  lua_settop(L, 2);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  float volume = float(luaL_checknumber(L, 2));
  self->setVolume(volume);
  return 0;
}

static int wrap_getVolume(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushnumber(L, self->getVolume());
  return 1;
}

static int wrap_setPan(lua_State* L) {
  lua_settop(L, 2);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  float pan = float(luaL_checknumber(L, 2));
  self->setPan(pan);
  return 0;
}

static int wrap_getPan(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushnumber(L, self->getPan());
  return 1;
}

static int wrap_setPitchShift(lua_State* L) {
  lua_settop(L, 2);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  float shift = float(luaL_checknumber(L, 2));
  self->setPitchShift(shift);
  return 0;
}

static int wrap_getPitchShift(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushnumber(L, self->getPitchShift());
  return 1;
}

static int wrap_isSeekable(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushboolean(L, self->isSeekable());
  return 1;
}

static void requireSeekable(lua_State* L, OutputStream* self) {
  if (!self->isSeekable())
    luaL_error(L, "not seekable");
}

static int wrap_getLength(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushinteger(L, self->getLength());
  return 1;
}

static int wrap_setPosition(lua_State* L) {
  lua_settop(L, 2);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  requireSeekable(L, self);
  int position = luaL_checkint(L, 2);
  self->setPosition(position);
  return 0;
}

static int wrap_getPosition(lua_State* L) {
  lua_settop(L, 1);
  OutputStream* self = luaAudiere_checkOutputStream(L, 1);
  lua_pushinteger(L, self->getPosition());
  return 1;
}

static const luaL_Reg OutputStream_m[] = {
  {"play", wrap_play},
  {"stop", wrap_stop},
  {"isPlaying", wrap_isPlaying},
  {"reset", wrap_reset},
  {"setRepeat", wrap_setRepeat},
  {"getRepeat", wrap_getRepeat},
  {"setVolume", wrap_setVolume},
  {"getVolume", wrap_getVolume},
  {"setPan", wrap_setPan},
  {"getPan", wrap_getPan},
  {"setPitchShift", wrap_setPitchShift},
  {"getPitchShift", wrap_getPitchShift},
  {"isSeekable", wrap_isSeekable},
  {"getLength", wrap_getLength},
  {"setPosition", wrap_setPosition},
  {"getPosition", wrap_getPosition},
  {0, 0}
};

LUA_AUDIERE_API void
luaAudiere_registerOutputStream(lua_State* L) {
  registerRefCounted(L);
  registerInterface(L, MT_OutputStream, MT_RefCounted, OutputStream_m,
                    "OutputStream");
}
