// *sigh*, looking forward to VS.NET...
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <string>
#include "audiere.h"
#include "debug.h"
#include "device_null.h"
#include "internal.h"
#include "threads.h"

#ifdef WIN32

  // include this before DS8, because it includes <dsound.h>, which defines
  // DIRECTSOUND_VERSION
  #include "device_ds3.h"

  #if DIRECTSOUND_VERSION >= 0x0800
    #include "device_ds8.h"
  #endif

#endif

#ifdef HAVE_OSS
  #include "device_oss.h"
#endif

#ifdef HAVE_OPENAL
  #include "device_al.h"
#endif


namespace audiere {

  #define NEED_SEMICOLON do ; while (false)

  #define TRY_GROUP(group_name) {                               \
    AudioDevice* device = DoOpenDevice(group_name, parameters); \
    if (device) {                                               \
      return device;                                            \
    }                                                           \
  } NEED_SEMICOLON

  #define TRY_DEVICE(DeviceType) {          \
    DeviceType* device = new DeviceType();  \
    if (device->initialize(parameters)) {   \
      return device;                        \
    } else {                                \
      delete device;                        \
    }                                       \
  } NEED_SEMICOLON


  AudioDevice* DoOpenDevice(
    const char* name,
    const char* parameters)
  {
    ADR_GUARD("DoOpenDevice");

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

      #ifdef HAVE_OPENAL
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

      if (dev == "" || dev == "autodetect") {
        TRY_GROUP("oss");
        TRY_GROUP("openal");
        return 0;
      }

      #ifdef HAVE_OSS
        if (dev == "oss") {
          TRY_DEVICE(OSSAudioDevice);
          return 0;
        }
      #endif

      #ifdef HAVE_OPENAL
        if (dev == "openal") {
          TRY_DEVICE(ALAudioDevice);
          return 0;
        }
      #endif

      if (dev == "null") {
        TRY_DEVICE(NullAudioDevice);
        return 0;
      }

    #endif

    // no devices
    return 0;
  }


  class ThreadedDevice : public DLLImplementation<AudioDevice> {
  public:
    ThreadedDevice(AudioDevice* device) {
      m_device = device;
      m_thread_exists = false;
      m_thread_should_die = false;

      /// @todo  what if thread creation fails?
      AI_CreateThread(threadRoutine, this, 0);
    }

    ~ThreadedDevice() {
      m_thread_should_die = true;
      while (m_thread_exists) {
        AI_Sleep(50);
      }

      delete m_device;
    }

    // don't need to update the device...  the thread does it for us
    void update() {
    }

    OutputStream* openStream(SampleSource* source) {
      return m_device->openStream(source);
    }

    OutputStream* openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format)
    {
      return m_device->openBuffer(
        samples, sample_count,
        channel_count, sample_rate, sample_format);
    }

  private:
    void run() {
      m_thread_exists = true;
      while (!m_thread_should_die) {
        m_device->update();
      }
      m_thread_exists = false;
    }

    static void threadRoutine(void* arg) {
      ThreadedDevice* This = (ThreadedDevice*)arg;
      This->run();
    }

  private:
    AudioDevice* m_device;
    volatile bool m_thread_should_die;
    volatile bool m_thread_exists;
  };


  ADR_EXPORT(AudioDevice*, AdrOpenDevice)(
    const char* name,
    const char* parameters)
  {
    ADR_GUARD("AdrOpenDevice");

    // first, we need an unthreaded audio device
    AudioDevice* device = DoOpenDevice(name, parameters);
    if (!device) {
      return 0;
    }

    return new ThreadedDevice(device);
  }

  ADR_EXPORT(AudioDevice3D*, AdrOpenDevice3D)(
    const char* name,
    const char* parameters)
  {
    return 0;
  }

}
