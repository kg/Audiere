#include <algorithm>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "device_oss.h"
#include "buffer_stream.h"
#include "debug.h"


namespace audiere {

  OSSAudioDevice*
  OSSAudioDevice::create(const ParameterList& parameters) {
    std::string device = parameters.getValue("device", "/dev/dsp");

    // attempt to open output device
    int output_device = open(device.c_str(), O_WRONLY);
    if (output_device == -1) {
      perror(device.c_str());
      return 0;
    }

    int format = AFMT_S16_LE;
    if (ioctl(output_device, SNDCTL_DSP_SETFMT, &format) == -1) {
      perror("SNDCTL_DSP_SETFMT");
      return 0;
    }
    if (format != AFMT_S16_LE) {
      // unsupported format
      return 0;
    }

    int stereo = 1;
    if (ioctl(output_device, SNDCTL_DSP_STEREO, &stereo) == -1) {
      perror("SNDCTL_DSP_STEREO");
      return 0;
    }
    if (stereo != 1) {
      // unsupported channel number
      return 0;
    }

    int speed = 44100;
    if (ioctl(output_device, SNDCTL_DSP_SPEED, &speed) == -1) {
      perror("SNDCTL_DSP_SPEED");
      return 0;
    }
    if (abs(44100 - speed) > 2205) {
      // unsupported sampling rate
      return 0;
    }

    return new OSSAudioDevice(output_device);
  }


  OSSAudioDevice::OSSAudioDevice(int output_device) {
    m_output_device = output_device;
  }


  OSSAudioDevice::~OSSAudioDevice() {
    ADR_GUARD("OSSAudioDevice::~OSSAudioDevice");

    if (m_output_device != -1) {
      close(m_output_device);
    }
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
    return new OSSOutputStream(this, source);
  }


  OutputStream*
  OSSAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, sample_count,
      channel_count, sample_rate, sample_format));
  }


  OSSOutputStream::OSSOutputStream(
    OSSAudioDevice* device,
    SampleSource* source)
  {
    m_device = device;
    m_source = source;

    getMixer().addSource(m_source.get());
  }


  OSSOutputStream::~OSSOutputStream() {
    getMixer().removeSource(m_source.get());
  }


  void
  OSSOutputStream::play() {
    getMixer().setPlaying(m_source.get(), true);
  }


  void
  OSSOutputStream::stop() {
    getMixer().setPlaying(m_source.get(), false);
  }


  bool
  OSSOutputStream::isPlaying() {
    return getMixer().isPlaying(m_source.get());
  }


  void
  OSSOutputStream::reset() {
    m_source->reset();
  }


  void
  OSSOutputStream::setRepeat(bool repeat) {
    /// @todo  implement OSSOutputStream::setRepeat
  }


  bool
  OSSOutputStream::getRepeat() {
    /// @todo  implement OSSOutputStream::getRepeat
  }


  void
  OSSOutputStream::setVolume(float volume) {
    m_volume = volume;
    getMixer().setVolume(m_source.get(), m_volume);
  }


  float
  OSSOutputStream::getVolume() {
    return m_volume;
  }


  void
  OSSOutputStream::setPan(float pan) {
    /// @todo  implement OSSOutputStream::setPan
  }


  float
  OSSOutputStream::getPan() {
    /// @todo  implement OSSOutputStream::getPan
    return 0.0f;
  }


  bool
  OSSOutputStream::isSeekable() {
    /// @todo  implement OSSOutputStream::isSeekable
    return false;
  }


  int
  OSSOutputStream::getLength() {
    /// @todo  implement OSSOutputStream::getLength
  }


  void
  OSSOutputStream::setPosition(int position) {
    /// @todo  implement OSSOutputStream::setPosition
  }


  int
  OSSOutputStream::getPosition() {
    /// @todo  implement OSSOutputStream::getPosition
    return 0;
  }


  Mixer&
  OSSOutputStream::getMixer() {
    return m_device->m_mixer;
  }

}
