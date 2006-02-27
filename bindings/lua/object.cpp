// Based on code written for Serotek Corporation (http://www.freedombox.info/)

#include <assert.h>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include "luaAudiere.h"
}

#include "luaAudiereTypes.h"
#include "internal.h"

void getMetatableByName(lua_State* L, const char* mtName) {
  assert(mtName != 0);
  luaL_getmetatable(L, mtName);
  assert(lua_istable(L, -1));
}

void setMetatable(lua_State* L, const char* mtName) {
  getMetatableByName(L, mtName);
  lua_setmetatable(L, -2);
}

bool newObjectType(lua_State* L, const char* mtName, const char* baseMTName,
                   const luaL_Reg* methods, const char* visibleName) {
  assert(mtName != 0);
  bool newMT = luaL_newmetatable(L, mtName) != 0; // always pushes a metatable
  if (newMT) { // this metatable didn't already exist
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    if (methods != 0)
      luaL_register(L, 0, methods);
    if (baseMTName != 0)
      setMetatable(L, baseMTName);
    if (visibleName != 0) {
      lua_pushstring(L, visibleName);
      lua_setfield(L, -2, "_visibleName");
    }
  }
  lua_pop(L, 1); // remove the result of luaL_newmetatable
  return newMT;
}

/* The expected metatable must be at the top.  The metatable name is used only
   as the default visible type name. */
static void objectTypeError(lua_State* L, int arg, const char* mtName) {
  lua_getfield(L, -1, "_visibleName");
  lua_remove(L, -2);
  const char* visibleName;
  if (lua_isstring(L, -1))
    visibleName = lua_tostring(L, -1);
  else {
    visibleName = mtName;
    lua_pop(L, 1);
  }
  const char* msg = lua_pushfstring(L, "%s object expected", visibleName);
  luaL_argerror(L, arg, msg);
}

static bool checkMetatable(lua_State* L) {
  bool rv;
  if (lua_rawequal(L, -1, -2)) { // compare received and expected metatables
    lua_pop(L, 2);
    rv = true;
  } else if (lua_getmetatable(L, -1)) {
    lua_remove(L, -2); // remove the metatable just checked
    rv = checkMetatable(L);
  } else {
    lua_pop(L, 1); // leave the expected metatable at the top
    rv = false;
  }
  return rv;
}

bool isObject(lua_State* L, int idx, const char* mtName) {
  assert(idx != 0);
  if (idx < 0)
    idx = lua_gettop(L) + idx + 1;
  getMetatableByName(L, mtName);
  if (!(lua_isuserdata(L, idx) && lua_getmetatable(L, idx) &&
        checkMetatable(L))) {
    lua_pop(L, 1);
    return false;
  }
  return true;
}

void* toObject(lua_State* L, int idx, const char* mtName) {
  if (isObject(L, idx, mtName))
    return lua_touserdata(L, idx);
  else
    return 0;
}

void* checkObject(lua_State* L, int arg, const char* mtName) {
  assert(arg > 0);
  /* We have to do this the long way, to leave the stack as objectTypeError
     expects it. */
  getMetatableByName(L, mtName);
  if (!(lua_isuserdata(L, arg) && lua_getmetatable(L, arg) &&
        checkMetatable(L)))
    objectTypeError(L, arg, mtName);
  return lua_touserdata(L, arg);
}

void initCacheTable(lua_State* L) {
  lua_pushvalue(L, -1); // re-push cache table name
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_istable(L, -1))
    lua_pop(L, 2); // remove both the table and the name
  else {
    lua_pop(L, 1);
    lua_newtable(L); // create the cache table
    lua_newtable(L); // create the cache table's metatable
    lua_pushliteral(L, "v"); // weak values
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }
}

bool isPointerCached(lua_State* L, void* p) {
  lua_rawget(L, LUA_REGISTRYINDEX);
  assert(lua_istable(L, -1));
  lua_pushlightuserdata(L, p);
  lua_rawget(L, -2);
  if (lua_isuserdata(L, -1)) {
    lua_remove(L, -2); // remove the cache table
    return true;
  } else {
    lua_pop(L, 1); // pop whatever we got from the cache (should be nil)
    return false;
  }
}

void newCachedPointer(lua_State* L, void* p, const char* mtName) {
  void** pp = static_cast<void**>(lua_newuserdata(L, sizeof(void*)));
  *pp = p;
  setMetatable(L, mtName);
  lua_pushlightuserdata(L, p);
  lua_pushvalue(L, -2); // re-push the full userdatum
  lua_rawset(L, -4);
  lua_remove(L, -2); // remove the cache table
}
