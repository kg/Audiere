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
luaAudiere_pushSampleSource(lua_State* L, SampleSource* obj) {
  pushRefCounted(L, obj, MT_SampleSource);
}

LUA_AUDIERE_API bool
luaAudiere_isSampleSource(lua_State* L, int idx) {
  return isRefCounted(L, idx, MT_SampleSource);
}

LUA_AUDIERE_API SampleSource*
luaAudiere_toSampleSource(lua_State* L, int idx) {
  return static_cast<SampleSource*>(toRefCounted(L, idx, MT_SampleSource));
}

LUA_AUDIERE_API SampleSource*
luaAudiere_checkSampleSource(lua_State* L, int arg) {
  return static_cast<SampleSource*>(checkRefCounted(L, arg, MT_SampleSource));
}

static int wrap_getFormat(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  int channel_count;
  int sample_rate;
  SampleFormat sample_format;
  self->getFormat(channel_count, sample_rate, sample_format);
  lua_pushinteger(L, channel_count);
  lua_pushinteger(L, sample_rate);
  lua_pushinteger(L, int(sample_format));
  return 3;
}

static int wrap_reset(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  self->reset();
  return 0;
}

static int wrap_isSeekable(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  lua_pushboolean(L, self->isSeekable());
  return 1;
}

static void requireSeekable(lua_State* L, SampleSource* self) {
  if (!self->isSeekable())
    luaL_error(L, "not seekable");
}

static int wrap_getLength(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  lua_pushinteger(L, self->getLength());
  return 1;
}

static int wrap_setPosition(lua_State* L) {
  lua_settop(L, 2);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  requireSeekable(L, self);
  int position = luaL_checkint(L, 2);
  self->setPosition(position);
  return 0;
}

static int wrap_getPosition(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  lua_pushinteger(L, self->getPosition());
  return 1;
}

static int wrap_getRepeat(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  lua_pushboolean(L, self->getRepeat());
  return 1;
}

static int wrap_setRepeat(lua_State* L) {
  lua_settop(L, 2);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  bool repeat = lua_toboolean(L, 2) != 0;
  self->setRepeat(repeat);
  return 0;
}

static int wrap_getTagCount(lua_State* L) {
  lua_settop(L, 1);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  lua_pushinteger(L, self->getTagCount());
  return 1;
}

static int checkTagIndex(lua_State* L, SampleSource* self, int arg) {
  int i = luaL_checkint(L, arg);
  luaL_argcheck(L, i >= 0, arg, "non-negative integer expected");
  luaL_argcheck(L, i < self->getTagCount(), arg, "index out of range");
  return i;
}

static int wrap_getTagKey(lua_State* L) {
  lua_settop(L, 2);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  int i = checkTagIndex(L, self, 2);
  lua_pushstring(L, self->getTagKey(i));
  return 1;
}

static int wrap_getTagValue(lua_State* L) {
  lua_settop(L, 2);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  int i = checkTagIndex(L, self, 2);
  lua_pushstring(L, self->getTagValue(i));
  return 1;
}

static int wrap_getTagType(lua_State* L) {
  lua_settop(L, 2);
  SampleSource* self = luaAudiere_checkSampleSource(L, 1);
  int i = checkTagIndex(L, self, 2);
  lua_pushstring(L, self->getTagType(i));
  return 1;
}

static const luaL_Reg SampleSource_m[] = {
  {"getFormat", wrap_getFormat},
  {"reset", wrap_reset},
  {"isSeekable", wrap_isSeekable},
  {"getLength", wrap_getLength},
  {"setPosition", wrap_setPosition},
  {"getPosition", wrap_getPosition},
  {"getRepeat", wrap_getRepeat},
  {"setRepeat", wrap_setRepeat},
  {"getTagCount", wrap_getTagCount},
  {"getTagKey", wrap_getTagKey},
  {"getTagValue", wrap_getTagValue},
  {"getTagType", wrap_getTagType},
  {0, 0}
};

LUA_AUDIERE_API void
luaAudiere_registerSampleSource(lua_State* L) {
  registerRefCounted(L);
  registerInterface(L, MT_SampleSource, MT_RefCounted, SampleSource_m,
                    "SampleSource");
}
