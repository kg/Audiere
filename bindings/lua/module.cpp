// Based on code written for Serotek Corporation (http://www.freedombox.info/)

#include <audiere.h>
#include <assert.h>
#include <string>
#include <vector>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include "luaAudiere.h"
}

#include "luaAudiereTypes.h"
#include "internal.h"

using namespace audiere;
using std::string;
using std::vector;

static int wrap_GetVersion(lua_State* L) {
  lua_settop(L, 0);
  lua_pushstring(L, GetVersion());
  return 1;
}

static int wrap_GetSupportedFileFormats(lua_State* L) {
  lua_settop(L, 0);
  vector<FileFormatDesc> formats;
  GetSupportedFileFormats(formats);
  unsigned n = formats.size();
  lua_createtable(L, n, 0);
  unsigned i;
  for (i = 0; i < n; i++) {
    const FileFormatDesc& format = formats[i];
    lua_createtable(L, 0, 2);
    lua_pushstring(L, format.description.c_str());
    lua_setfield(L, -2, "description");
    const vector<string>& extensions = format.extensions;
    unsigned n2 = extensions.size();
    lua_createtable(L, n2, 0);
    unsigned j;
    for (j = 0; j < n2; j++) {
      lua_pushstring(L, extensions[j].c_str());
      lua_rawseti(L, -2, j + 1);
    }
    lua_setfield(L, -2, "extensions");
    lua_rawseti(L, -2, i + 1);
  }
  return 1;
}

static int wrap_GetSupportedAudioDevices(lua_State* L) {
  lua_settop(L, 0);
  vector<AudioDeviceDesc> devices;
  GetSupportedAudioDevices(devices);
  unsigned n = devices.size();
  lua_createtable(L, n, 0);
  unsigned i;
  for (i = 0; i < n; i++) {
    const AudioDeviceDesc& device = devices[i];
    lua_createtable(L, 0, 2);
    lua_pushstring(L, device.name.c_str());
    lua_setfield(L, -2, "name");
    lua_pushstring(L, device.description.c_str());
    lua_setfield(L, -2, "description");
    lua_rawseti(L, -2, i + 1);
  }
  return 1;
}

static int wrap_GetSampleSize(lua_State* L) {
  lua_settop(L, 1);
  SampleFormat format = SampleFormat(luaL_checkint(L, 1));
  lua_pushinteger(L, GetSampleSize(format));
  return 1;
}

static int wrap_OpenDevice(lua_State* L) {
  lua_settop(L, 2);
  const char* name = luaL_optstring(L, 1, 0);
  const char* parameters = luaL_optstring(L, 2, 0);
  luaAudiere_pushAudioDevice(L, OpenDevice(name, parameters));
  return 1;
}

static int wrap_OpenSampleSource(lua_State *L) {
  lua_settop(L, 2);
  FileFormat file_format = FileFormat(luaL_optinteger(L, 2, FF_AUTODETECT));
  SampleSource* rv = 0;
  if (lua_isstring(L, 1)) {
    const char* filename = lua_tostring(L, 1);
    rv = OpenSampleSource(filename, file_format);
  } else if (luaAudiere_isFile(L, 1)) {
    File* file = luaAudiere_toFile(L, 1);
    rv = OpenSampleSource(file, file_format);
  } else
    luaL_argerror(L, 1, "expected a string or File");
  luaAudiere_pushSampleSource(L, rv);
  return 1;
}

static int wrap_CreateTone(lua_State* L) {
  lua_settop(L, 1);
  double frequency = luaL_checknumber(L, 1);
  luaAudiere_pushSampleSource(L, CreateTone(frequency));
  return 1;
}

static int wrap_CreateSquareWave(lua_State* L) {
  lua_settop(L, 1);
  double frequency = luaL_checknumber(L, 1);
  luaAudiere_pushSampleSource(L, CreateSquareWave(frequency));
  return 1;
}

static int wrap_CreateWhiteNoise(lua_State* L) {
  lua_settop(L, 0);
  luaAudiere_pushSampleSource(L, CreateWhiteNoise());
  return 1;
}

static int wrap_CreatePinkNoise(lua_State* L) {
  lua_settop(L, 0);
  luaAudiere_pushSampleSource(L, CreatePinkNoise());
  return 1;
}

static int wrap_CreateLoopPointSource(lua_State* L) {
  lua_settop(L, 2);
  LoopPointSource* rv = 0;
  if (luaAudiere_isSampleSource(L, 1)) {
    SampleSource* source = luaAudiere_toSampleSource(L, 1);
    rv = CreateLoopPointSource(source);
  } else if (lua_isstring(L, 1)) {
    const char* filename = lua_tostring(L, 1);
    FileFormat file_format = FileFormat(luaL_optinteger(L, 2, FF_AUTODETECT));
    rv = CreateLoopPointSource(filename, file_format);
  } else if (luaAudiere_isFile(L, 1)) {
    File* file = luaAudiere_toFile(L, 1);
    FileFormat file_format = FileFormat(luaL_optinteger(L, 2, FF_AUTODETECT));
    rv = CreateLoopPointSource(file, file_format);
  } else
    luaL_argerror(L, 1, "expected a SampleSource, string, or File");
  luaAudiere_pushLoopPointSource(L, rv);
  return 1;
}

static int wrap_OpenSound(lua_State* L) {
  lua_settop(L, 4);
  AudioDevice* device = luaAudiere_checkAudioDevice(L, 1);
  bool streaming = lua_toboolean(L, 3) != 0;
  OutputStream* rv = 0;
  if (luaAudiere_isSampleSource(L, 2)) {
    SampleSource* source = luaAudiere_toSampleSource(L, 2);
    rv = OpenSound(device, source, streaming);
  } else if (lua_isstring(L, 2)) {
    const char* filename = lua_tostring(L, 2);
    FileFormat file_format = FileFormat(luaL_optinteger(L, 4, FF_AUTODETECT));
    rv = OpenSound(device, filename, streaming, file_format);
  } else if (luaAudiere_isFile(L, 1)) {
    File* file = luaAudiere_toFile(L, 2);
    FileFormat file_format = FileFormat(luaL_optinteger(L, 4, FF_AUTODETECT));
    rv = OpenSound(device, file, streaming, file_format);
  } else
    luaL_argerror(L, 2, "expected a SampleSource, string, or File");
  luaAudiere_pushOutputStream(L, rv);
  return 1;
}

static int wrap_CreateMemoryFile(lua_State* L) {
  lua_settop(L, 1);
  size_t size;
  const char* buffer = luaL_checklstring(L, 1, &size);
  luaAudiere_pushFile(L, CreateMemoryFile(buffer, int(size)));
  return 1;
}

static const luaL_Reg audierelib_f[] = {
  {"GetVersion", wrap_GetVersion},
  {"GetSupportedFileFormats", wrap_GetSupportedFileFormats},
  {"GetSupportedAudioDevices", wrap_GetSupportedAudioDevices},
  {"GetSampleSize", wrap_GetSampleSize},
  {"OpenDevice", wrap_OpenDevice},
  {"OpenSampleSource", wrap_OpenSampleSource},
  {"CreateTone", wrap_CreateTone},
  {"CreateSquareWave", wrap_CreateSquareWave},
  {"CreateWhiteNoise", wrap_CreateWhiteNoise},
  {"CreatePinkNoise", wrap_CreatePinkNoise},
  {"CreateLoopPointSource", wrap_CreateLoopPointSource},
  {"OpenSound", wrap_OpenSound},
  {"CreateMemoryFile", wrap_CreateMemoryFile},
  {0, 0}
};

struct intconst {
  const char* name;
  lua_Integer value;
};

static const struct intconst int_constants[] = {
  // SampleFormat
  {"SF_U8", SF_U8},
  {"SF_S16", SF_S16},
  // FileFormat
  {"FF_AUTODETECT", FF_AUTODETECT},
  {"FF_WAV", FF_WAV},
  {"FF_OGG", FF_OGG},
  {"FF_FLAC", FF_FLAC},
  {"FF_MP3", FF_MP3},
  {"FF_MOD", FF_MOD},
  {"FF_AIFF", FF_AIFF},
  {"FF_SPEEX", FF_SPEEX},
  {0, 0}
};

LUA_AUDIERE_API int luaopen_audiere(lua_State* L) {
  luaAudiere_registerAudioDevice(L);
  luaAudiere_registerSampleSource(L);
  luaAudiere_registerFile(L);
  luaAudiere_registerLoopPointSource(L);
  luaAudiere_registerOutputStream(L);
  luaL_register(L, "audiere", audierelib_f);
  const intconst* ic;
  for (ic = int_constants; ic->name; ic++) {
    lua_pushinteger(L, ic->value);
    lua_setfield(L, -2, ic->name);
  }
  return 1;
}
