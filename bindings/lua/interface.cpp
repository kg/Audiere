// Based on code written for Serotek Corporation (http://www.freedombox.info/)

#include <assert.h>
#include <audiere.h>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include "luaAudiere.h"
}

#include "luaAudiereTypes.h"
#include "internal.h"

using namespace audiere;

#define OBJCACHE_PREFIX "luaAudiere:objcache:"

static RefCounted** toBoxedRefCounted(lua_State* L, int idx,
                                      const char* mtName) {
  return static_cast<RefCounted**>(toObject(L, idx, mtName));
}

static RefCounted** checkBoxedRefCounted(lua_State* L, int arg,
                                         const char* mtName) {
  return static_cast<RefCounted**>(checkObject(L, arg, mtName));
}

static int gcRefCounted(lua_State *L) {
  lua_settop(L, 1);
  RefCounted** pobj = checkBoxedRefCounted(L, 1, MT_RefCounted);
  RefCounted* obj = *pobj;
  if (obj != 0) {
    obj->unref();
    *pobj = 0;
  }
  return 0;
}

void registerInterface(lua_State* L, const char* mtName,
                       const char* baseMTName, const luaL_Reg* methods,
                       const char* visibleName) {
  if (newObjectType(L, mtName, baseMTName, methods, visibleName)) {
    getMetatableByName(L, mtName);
    lua_pushcfunction(L, gcRefCounted);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1); // remove metatable
    lua_pushliteral(L, OBJCACHE_PREFIX);
    lua_pushstring(L, mtName);
    lua_concat(L, 2);
    initCacheTable(L);
  }
}

void pushRefCounted(lua_State* L, RefCounted* obj, const char* mtName) {
  if (obj == 0)
    lua_pushnil(L);
  else {
    lua_pushliteral(L, OBJCACHE_PREFIX);
    lua_pushstring(L, mtName);
    lua_concat(L, 2);
    if (!isPointerCached(L, obj)) {
      obj->ref();
      newCachedPointer(L, obj, mtName);
    }
  }
}

bool isRefCounted(lua_State* L, int idx, const char* mtName) {
  RefCounted** pp = toBoxedRefCounted(L, idx, mtName);
  return pp != 0 && *pp != 0;
}

RefCounted* toRefCounted(lua_State* L, int idx, const char* mtName) {
  RefCounted** pp = toBoxedRefCounted(L, idx, mtName);
  if (pp == 0)
    return 0;
  else
    return *pp;
}

RefCounted* checkRefCounted(lua_State* L, int arg, const char* mtName) {
  RefCounted** pp = checkBoxedRefCounted(L, arg, mtName);
  luaL_argcheck(L, *pp != 0, arg, "object already released");
  return *pp;
}

void registerRefCounted(lua_State* L) {
  registerInterface(L, MT_RefCounted, 0, 0, "RefCounted");
}
