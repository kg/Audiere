#include <algorithm>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "device_oss.h"
#include "debug.h"


namespace audiere {

  OSSAudioDevice::OSSAudioDevice() {
    m_output_device = -1;
  }


  OSSAudioDevice::~OSSAudioDevice() {
    ADR_GUARD("OSSAudioDevice::~OSSAudioDevice");

    if (m_output_device != -1) {
      close(m_output_device);
    }
  }


  bool
  OSSAudioDevice::initialize(const char* parameters) {
    ParameterList pl(ParseParameters(parameters));

    std::string device = "/dev/dsp";  // default device

    ParameterList::iterator i;
    for (i = pl.begin(); i != pl.end(); ++i) {
      if (i->first == "device") {
        device = i->second;
      }
    }

    // attempt to open output device
    m_output_device = open(device.c_str(), O_WRONLY);
    if (m_output_device == -1) {
      perror(device.c_str());
      return false;
    }

    int format = AFMT_S16_LE;
    if (ioctl(m_output_device, SNDCTL_DSP_SETFMT, &format) == -1) {
      perror("SNDCTL_DSP_SETFMT");
      return false;
    }
    if (format != AFMT_S16_LE) {
      // unsupported format
      return false;
    }

    int stereo = 1;
    if (ioctl(m_output_device, SNDCTL_DSP_STEREO, &stereo) == -1) {
      perror("SNDCTL_DSP_STEREO");
      return false;
    }
    if (stereo != 1) {
      // unsupported channel number
      return false;
    }

    int speed = 44100;
    if (ioctl(m_output_device, SNDCTL_DSP_SPEED, &speed) == -1) {
      perror("SNDCTL_DSP_SPEED");
      return false;
    }
    if (abs(44100 - speed) > 2205) {
      // unsupported sampling rate
      return false;
    }

    return true;
  }


  void
  OSSAudioDevice::update() {
    // find out how much data we can write to the device before it blocks
    audio_buf_info info;
    if (ioctl(m_output_device, SNDCTL_DSP_GETOSPACE, &info) == -1) {
      return;
    }

    // how many samples is that?
    int sample_count = info.bytes / 4;

    // write to the OSS device, 1024 samples at a time
    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE * 4];
    while (sample_count > 0) {
      int transfer_count = std::min(sample_count, BUFFER_SIZE);

      m_mixer.read(transfer_count, buffer);
      write(m_output_device, buffer, transfer_count * 4);

      sample_count -= transfer_count;
    }

    usleep(50000);  // 5 milliseconds
  }


  OutputStream*
  OSSAudioDevice::openStream(SampleSource* source) {
    return new OSSOutputStream(&m_mixer, source);
  }


  OSSOutputStream::OSSOutputStream(Mixer* mixer, SampleSource* source) {
    m_mixer      = mixer;
    m_source     = source;

    m_mixer->addSource(source);
  }


  OSSOutputStream::~OSSOutputStream() {
    m_mixer->removeSource(m_source);
  }


  void
  OSSOutputStream::play() {
    m_mixer->setPlaying(m_source, true);
  }


  void
  OSSOutputStream::stop() {
    m_mixer->setPlaying(m_source, false);
  }


  void
  OSSOutputStream::reset() {
    m_source->reset();
  }


  bool
  OSSOutputStream::isPlaying() {
    return m_mixer->isPlaying(m_source);
  }


  void
  OSSOutputStream::setVolume(int volume) {
    m_volume = volume;
    m_mixer->setVolume(m_source, m_volume);
  }


  int
  OSSOutputStream::getVolume() {
    return m_volume;
  }

}
