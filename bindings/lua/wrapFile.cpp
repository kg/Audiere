// Based on code written for Serotek Corporation (http://www.freedombox.info/)

#include <audiere.h>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include "luaAudiere.h"
}

#include "luaAudiereTypes.h"
#include "internal.h"

using namespace audiere;

LUA_AUDIERE_API void
luaAudiere_pushFile(lua_State* L, File* obj) {
  pushRefCounted(L, obj, MT_File);
}

LUA_AUDIERE_API bool
luaAudiere_isFile(lua_State* L, int idx) {
  return isRefCounted(L, idx, MT_File);
}

LUA_AUDIERE_API File*
luaAudiere_toFile(lua_State* L, int idx) {
  return static_cast<File*>(toRefCounted(L, idx, MT_File));
}

LUA_AUDIERE_API File*
luaAudiere_checkFile(lua_State* L, int arg) {
  return static_cast<File*>(checkRefCounted(L, arg, MT_File));
}

LUA_AUDIERE_API void
luaAudiere_registerFile(lua_State* L) {
  registerRefCounted(L);
  registerInterface(L, MT_File, MT_RefCounted, 0, "File");
}
