#include <errno.h>
#include <unistd.h>
#include "device_al.h"


namespace audiere {

  ALAudioDevice*
  ALAudioDevice::create(const ParameterList& parameters) {

    // if anything goes wrong, assume 44100 Hz
    int rate = 44100;

    int device = alGetResourceByName(AL_SYSTEM, "DefaultOut", AL_DEVICE_TYPE);
    if (device) {
      ALpv pv;
      pv.param = AL_RATE;
      if (-1 == alGetParams(device, &pv, 1)) {
        fprintf(stderr, "Couldn't get rate: %s\n",
                alGetErrorString(oserror()));
      }
      rate = pv.value.i;
    } else {
      fprintf(stderr, "Couldn't get DefaultOut resource: %s\n",
              alGetErrorString(oserror()));
    }

    ALconfig config = alNewConfig();
    if (!config) {
      fprintf(stderr, "Couldn't create ALconfig: %s\n",
              alGetErrorString(oserror()));
      return 0;
    }

    // stereo
    int result = alSetChannels(config, 2);
    if (result != 0) {
      fprintf(stderr, "Couldn't set channels: %s\n",
              alGetErrorString(oserror()));
      alFreeConfig(config);
      return 0;
    }

    ALport port = alOpenPort("Audiere Output Port", "w", config);
    if (!port) {
      fprintf(stderr, "Couldn't open port: %s\n",
              alGetErrorString(oserror()));
      alFreeConfig(config);
      return 0;
    }

    alFreeConfig(config);
    return new ALAudioDevice(port, rate);
  }


  ALAudioDevice::ALAudioDevice(ALport port, int rate)
    : MixerDevice(rate)
  {
    m_port = port;
  }


  ALAudioDevice::~ALAudioDevice() {
    alClosePort(m_port);
  }


  void
  ALAudioDevice::update() {
    // how much data can we write?
    const int filled = alGetFilled(m_port);
    int can_write = 50000 - filled;  // empty portion of the buffer

    // write 1024 frames at a time
    static const int BUFFER_SIZE = 1024;
    u8 buffer[BUFFER_SIZE * 4];
    while (can_write > 0) {
      int transfer_count = std::min(can_write, BUFFER_SIZE);

      read(transfer_count, buffer);
      alWriteFrames(m_port, buffer, transfer_count);
      can_write -= transfer_count;
    }

    usleep(50000);  // 50 milliseconds
  }

}
