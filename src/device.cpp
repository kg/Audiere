// *sigh*, looking forward to VS.NET...
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <string>
#include "audiere.h"
#include "debug.h"
#include "device_null.h"
#include "internal.h"

#ifdef WIN32

  // include this before DS8, because it includes <dsound.h>, which defines
  // DIRECTSOUND_VERSION
  #include "device_ds3.h"

  #if DIRECTSOUND_VERSION >= 0x0800
    #include "device_ds8.h"
  #endif

#endif

#ifdef WITH_OSS
  #include "device_oss.h"
#endif

#ifdef WITH_OPENAL
  #include "device_al.h"
#endif


namespace audiere {

  #define NEED_SEMICOLON do ; while (false)

  #define TRY_GROUP(group_name) {                                       \
    AudioDevice* device = OpenDevice(group_name, parameters, threaded); \
    if (device) {                                                       \
      return device;                                                    \
    }                                                                   \
  } NEED_SEMICOLON

  #define TRY_DEVICE(DeviceType) {          \
    DeviceType* device = new DeviceType();  \
    if (device->initialize(parameters)) {   \
      return device;                        \
    } else {                                \
      delete device;                        \
    }                                       \
  } NEED_SEMICOLON


  ADR_EXPORT(AudioDevice*, AdrOpenDevice)(
    const char* name,
    const char* parameters,
    bool threaded)
  {
    ADR_GUARD("AdrOpenDevice");

    if (!name) {
      name = "";
    }
    if (!parameters) {
      parameters = "";
    }


    std::string dev(name);

    #ifdef WIN32

      if (dev == "" || dev == "autodetect") {
        TRY_GROUP("directsound");
        TRY_GROUP("openal");
        return 0;
      }

      if (dev == "directsound") {
        #if DIRECTSOUND_VERSION >= 0x0800
          TRY_DEVICE(DS8AudioDevice);
        #endif
        TRY_DEVICE(DS3AudioDevice);
        return 0;
      }

      #ifdef WITH_OPENAL
        if (dev == "openal") {
          TRY_DEVICE(ALAudioDevice);
          return 0;
        }
      #endif

      if (dev == "null") {
        TRY_DEVICE(NullAudioDevice);
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
          TRY_DEVICE(OSSOutputContext);
          return 0;
        }
      #endif

      #ifdef WITH_OPENAL
        if (dev == "openal") {
          TRY_DEVICE(ALOutputContext);
          return 0;
        }
      #endif

      if (dev == "null") {
        TRY_DEVICE(NullOutputContext);
        return 0;
      }

    #endif

    // no devices
    return 0;
  }

}
