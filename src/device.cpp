// *sigh*, looking forward to VS.NET...
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <string>
#include "audiere.h"
#include "debug.hpp"
#include "device_null.hpp"
#include "internal.hpp"

#ifdef WIN32

  // include this before DS8, because it includes <dsound.h>, which defines
  // DIRECTSOUND_VERSION
  #include "device_ds3.hpp"

  #if DIRECTSOUND_VERSION >= 0x0800
    #include "device_ds8.hpp"
  #endif

#endif

#ifdef WITH_OSS
  #include "device_oss.hpp"
#endif

#ifdef WITH_OPENAL
  #include "device_al.hpp"
#endif


namespace audiere {

  template<typename T>
  T* TryDevice(const char* parameters) {
    T* device = new T();
    if (device->initialize(parameters)) {
      return device;
    } else {
      delete device;
      return 0;
    }
  }


  ADR_EXPORT(AudioDevice*, AdrOpenDevice)(
    const char* name,
    const char* parameters,
    bool threaded)
  {
    ADR_GUARD("AdrOpenDevice");


    #define NEED_SEMICOLON do ; while (false)

    #define TRY_GROUP(group_name) {                                       \
      AudioDevice* device = OpenDevice(group_name, parameters, threaded); \
      if (device) {                                                       \
        return device;                                                    \
      }                                                                   \
    } NEED_SEMICOLON

    #define TRY_CONTEXT(device_type) {                          \
      device_type* device = TryDevice<device_type>(parameters); \
      if (device) {                                             \
        return device;                                          \
      }                                                         \
    } NEED_SEMICOLON


    std::string dev(name);

    #ifdef WIN32

      if (dev == "" || dev == "autodetect") {
        TRY_GROUP("directsound");
        TRY_GROUP("openal");
        return 0;
      }

      if (dev == "directsound") {
        #if DIRECTSOUND_VERSION >= 0x0800
          TRY_CONTEXT(DS8AudioDevice);
        #endif
        TRY_CONTEXT(DS3AudioDevice);
        return 0;
      }

      #ifdef WITH_OPENAL
        if (dev == "openal") {
          TRY_CONTEXT(ALAudioDevice);
          return 0;
        }
      #endif

      if (dev == "null") {
        TRY_CONTEXT(NullAudioDevice);
        return 0;
      }

    #else  // not Win32 - assume autoconf UNIX

      if (dev == "" || device == "autodetect") {
        TRY_GROUP("oss");
        TRY_GROUP("openal");
        return 0;
      }

      #ifdef WITH_OSS
        if (dev == "oss") {
          TRY_CONTEXT(OSSOutputContext);
          return 0;
        }
      #endif

      #ifdef WITH_OPENAL
        if (dev == "openal") {
          TRY_CONTEXT(ALOutputContext);
          return 0;
        }
      #endif

      if (dev == "null") {
        TRY_CONTEXT(NullOutputContext);
        return 0;
      }

    #endif

    // no devices
    return 0;
  }


}
