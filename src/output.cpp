// *sigh*, looking forward to VS.NET...
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <string.h>
#include "debug.hpp"
#include "output_null.hpp"

#ifdef _WIN32

  // include this before DS8, because it includes <dsound.h>, which defines
  // DIRECTSOUND_VERSION
  #include "output_ds3.hpp"

  #if DIRECTSOUND_VERSION >= 0x0800
    #include "output_ds8.hpp"
  #endif

#endif

#ifdef WITH_OSS
  #include "output_oss.hpp"
#endif

#ifdef WITH_OPENAL
  #include "output_al.hpp"
#endif


////////////////////////////////////////////////////////////////////////////////

template<typename T>
static T* TryOutputContext(const char* parameters)
{
  T* context = new T();
  if (context->Initialize(parameters)) {
    return context;
  } else {
    delete context;
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

IOutputContext* OpenContext(const char* device, const char* parameters)
{
  ADR_GUARD("OpenContext");

  IOutputContext* context;

  #define TRY_CONTEXT(context_type)                         \
    context = TryOutputContext<context_type>(parameters);   \
    if (context) {                                          \
      return context;                                       \
    }

#ifdef _WIN32

  // autodetect
  if (strcmp(device, "") == 0 ||
      strcmp(device, "autodetect") == 0) {

    #if DIRECTSOUND_VERSION >= 0x0800
      TRY_CONTEXT(DS8OutputContext)
    #endif
    TRY_CONTEXT(DS3OutputContext)

  // DirectSound
  } else if (strcmp(device, "directsound") == 0) {

    #if DIRECTSOUND_VERSION >= 0x0800
    TRY_CONTEXT(DS8OutputContext)
    #endif
    TRY_CONTEXT(DS3OutputContext)

#ifdef WITH_OPENAL

  // OpenAL
  } else if (strcmp(device, "openal") == 0) {
  
    TRY_CONTEXT(ALOutputContext)

#endif

  // null
  } else if (strcmp(device, "null") == 0) {

    TRY_CONTEXT(NullOutputContext);

  }

#else  // Not WIN32

  if (strcmp(device, "") == 0 ||
      strcmp(device, "autodetect") == 0) {

#ifdef WITH_OSS
    TRY_CONTEXT(OSSOutputContext);
#endif

#ifdef WITH_OPENAL
    TRY_CONTEXT(ALOutputContext);
#endif

#ifdef WITH_OSS
  } else if (strcmp(device, "oss") == 0) {
    TRY_CONTEXT(OSSOutputContext);
#endif

#ifdef WITH_OPENAL
  } else if (strcmp(device, "openal") == 0) {
    TRY_CONTEXT(ALOutputContext);
#endif
  } else if (strcmp(device, "null") == 0) {
    TRY_CONTEXT(NullOutputContext);
  }

#endif

  // otherwise, just fail
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void ParseParameters(const char* parameter_string, ParameterList& parameters)
{
  std::string key;
  std::string value;

  std::string* current_string = &key;

  // walk the string and generate the parameter list
  const char* p = parameter_string;
  while (*p) {

    if (*p == '=') {

      current_string = &value;

    } else if (*p == ',') {

      if (key.length() && value.length()) {
        parameters.push_back(Parameter(key, value));
      }
      key   = "";
      value = "";
      current_string = &key;

    } else {
      *current_string += *p;
    }

    ++p;
  }

  // is there one more parameter without a trailing comma?
  if (key.length() && value.length()) {
    parameters.push_back(Parameter(key, value));
  }
}

////////////////////////////////////////////////////////////////////////////////
