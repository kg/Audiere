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

  #include <windows.h>
  #include <mmsystem.h>
  #include "device_ds.h"
  #include "device_mm.h"

#endif

#ifdef HAVE_OSS
  #include "device_oss.h"
#endif

#ifdef HAVE_OPENAL
  #include "device_oal.h"
#endif

#ifdef HAVE_AL
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

  #define TRY_DEVICE(DeviceType) {                         \
    DeviceType* device = DeviceType::create(parameters);   \
    if (device) {                                          \
      return device;                                       \
    }                                                      \
  } NEED_SEMICOLON


  AudioDevice* DoOpenDevice(
    const std::string& name,
    const ParameterList& parameters)
  {
    ADR_GUARD("DoOpenDevice");

    #ifdef WIN32

      if (name == "" || name == "autodetect") {
        TRY_GROUP("directsound");
        TRY_GROUP("winmm");
        TRY_GROUP("openal");
        return 0;
      }

      if (name == "directsound") {
        TRY_DEVICE(DSAudioDevice);
        return 0;
      }

      if (name == "winmm") {
        TRY_DEVICE(MMAudioDevice);
        return 0;
      }

      #ifdef HAVE_OPENAL
        if (name == "openal") {
          TRY_DEVICE(ALAudioDevice);
          return 0;
        }
      #endif

      if (name == "null") {
        TRY_DEVICE(NullAudioDevice);
        return 0;
      }

    #else  // not Win32 - assume autoconf UNIX

      if (name == "" || name == "autodetect") {
        // in decreasing order of sound API quality
        TRY_GROUP("al");
        TRY_GROUP("oss");
        TRY_GROUP("openal");
        return 0;
      }

      #ifdef HAVE_OSS
        if (name == "oss") {
          TRY_DEVICE(OSSAudioDevice);
          return 0;
        }
      #endif

      #ifdef HAVE_OPENAL
        if (name == "openal") {
          TRY_DEVICE(OALAudioDevice);
          return 0;
        }
      #endif

      #ifdef HAVE_AL
        if (name == "al") {
          TRY_DEVICE(ALAudioDevice);
          return 0;
        }
      #endif

      if (name == "null") {
        TRY_DEVICE(NullAudioDevice);
        return 0;
      }

    #endif

    // no devices
    return 0;
  }


  class ThreadedDevice : public RefImplementation<AudioDevice> {
  public:
    ThreadedDevice(AudioDevice* device) {
      ADR_GUARD("ThreadedDevice::ThreadedDevice");
      if (device) {
        ADR_LOG("Device is valid");
      } else {
        ADR_LOG("Device is not valid");
      }

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
    }

    // don't need to update the device...  the thread does it for us
    void ADR_CALL update() {
    }

    OutputStream* ADR_CALL openStream(SampleSource* source) {
      return m_device->openStream(source);
    }

    OutputStream* ADR_CALL openBuffer(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format)
    {
      return m_device->openBuffer(
        samples, frame_count,
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
      ADR_GUARD("ThreadedDevice::threadRoutine");
      if (arg) {
        ADR_LOG("arg is valid");
      } else {
        ADR_LOG("arg is not valid");
      }

      ThreadedDevice* This = (ThreadedDevice*)arg;
      This->run();
    }

  private:
    RefPtr<AudioDevice> m_device;
    volatile bool m_thread_should_die;
    volatile bool m_thread_exists;
  };


  ADR_EXPORT(AudioDevice*, AdrOpenDevice)(
    const char* name,
    const char* parameters)
  {
    ADR_GUARD("AdrOpenDevice");

    if (!name) {
      name = "";
    }
    if (!parameters) {
      parameters = "";
    }

    // first, we need an unthreaded audio device
    AudioDevice* device = DoOpenDevice(
      std::string(name),
      ParameterList(parameters));
    if (!device) {
      ADR_LOG("Could not open device");
      return 0;
    }

    ADR_LOG("creating threaded device");
    return new ThreadedDevice(device);
  }

}
