#ifndef luaAudiere_h
#define luaAudiere_h

#include <lua.h>

#ifdef LUA_AUDIERE_BUILD_AS_DLL
#ifdef LUA_AUDIERE_IMPL
#define LUA_AUDIERE_API __declspec(dllexport)
#else
#define LUA_AUDIERE_API __declspec(dllimport)
#endif
#endif

#ifndef LUA_AUDIERE_API
#define LUA_AUDIERE_API extern
#endif

LUA_AUDIERE_API int luaopen_audiere(lua_State *L);

#endif
