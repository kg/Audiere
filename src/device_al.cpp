#include <errno.h>
#include <unistd.h>
#include "device_al.h"
#include "buffer_stream.h"


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

    // find the output rate
    ALpv pv;
    pv.param = AL_RATE;

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


  ALAudioDevice::ALAudioDevice(ALport port, int rate) {
    m_port = port;
    m_mixer = new Mixer(rate);
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

      m_mixer->read(transfer_count, buffer);
      alWriteFrames(m_port, buffer, transfer_count);
      can_write -= transfer_count;
    }

    usleep(50000);  // 50 milliseconds
  }

  OutputStream*
  ALAudioDevice::openStream(SampleSource* source) {
    return (source ? new ALOutputStream(this, source) : 0);
  }


  OutputStream*
  ALAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, sample_count,
      channel_count, sample_rate, sample_format));
  }


  ALOutputStream::ALOutputStream(
    ALAudioDevice* device,
    SampleSource* source)
  {
    m_device = device;
    m_source = source;

    getMixer().addSource(m_source);
  }

  ALOutputStream::~ALOutputStream() {
    getMixer().removeSource(m_source);
  }


  void
  ALOutputStream::play() {
    getMixer().setPlaying(m_source, true);
  }


  void
  ALOutputStream::stop() {
    getMixer().setPlaying(m_source, false);
  }


  bool
  ALOutputStream::isPlaying() {
    return getMixer().isPlaying(m_source);
  }


  void
  ALOutputStream::reset() {
    getMixer().resetSource(m_source);
  }


  void
  ALOutputStream::setRepeat(bool repeat) {
    getMixer().setRepeat(m_source, repeat);
  }


  bool
  ALOutputStream::getRepeat() {
    return getMixer().getRepeat(m_source);
  }


  void
  ALOutputStream::setVolume(float volume) {
    getMixer().setVolume(m_source, volume);
  }


  float
  ALOutputStream::getVolume() {
    return getMixer().getVolume(m_source);
  }


  void
  ALOutputStream::setPan(float pan) {
    getMixer().setPan(m_source, pan);
  }


  float
  ALOutputStream::getPan() {
    return getMixer().getPan(m_source);
  }


  bool
  ALOutputStream::isSeekable() {
    /// @todo  implement ALOutputStream::isSeekable
    return false;
  }


  int
  ALOutputStream::getLength() {
    /// @todo  implement ALOutputStream::getLength
    return 0;
  }


  void
  ALOutputStream::setPosition(int position) {
    /// @todo  implement ALOutputStream::setPosition
  }


  int
  ALOutputStream::getPosition() {
    /// @todo  implement ALOutputStream::getPosition
    return 0;
  }


  Mixer&
  ALOutputStream::getMixer() {
    return *(m_device->m_mixer);
  }

}
