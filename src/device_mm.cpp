#include "device_mm.h"
#include "buffer_stream.h"
#include <mmreg.h>


namespace audiere {

  MMAudioDevice*
  MMAudioDevice::create(const ParameterList& parameters) {
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = 2;
    wfx.nSamplesPerSec  = 44100;
    wfx.nAvgBytesPerSec = 44100 * 4;
    wfx.nBlockAlign     = 4;
    wfx.wBitsPerSample  = 16;
    wfx.cbSize          = sizeof(WAVEFORMATEX);

    HWAVEOUT handle;
    MMRESULT result = waveOutOpen(&handle, WAVE_MAPPER, &wfx, 0, 0, 0);
    if (result != MMSYSERR_NOERROR) {
      return 0;
    }

    return new MMAudioDevice(handle);
  }


  MMAudioDevice::MMAudioDevice(HWAVEOUT device) {
    ADR_GUARD("MMAudioDevice::MMAudioDevice");

    m_device = device;
    m_mixer = new Mixer(44100);
    m_current_buffer = 0;

    // fill each buffer with samples and prepare it for output
    for (int i = 0; i < BUFFER_COUNT; ++i) {
      WAVEHDR& wh = m_buffers[i];
      memset(&wh, 0, sizeof(wh));
      wh.lpData         = (char*)m_samples + i * BUFFER_LENGTH;
      wh.dwBufferLength = BUFFER_LENGTH;

      m_mixer->read(BUFFER_LENGTH / 4, wh.lpData);

      MMRESULT result = waveOutPrepareHeader(m_device, &wh, sizeof(wh));
      if (result != MMSYSERR_NOERROR) {
        ADR_LOG("waveOutPrepareHeader failed");
      }

      result = waveOutWrite(m_device, &wh, sizeof(wh));
      if (result != MMSYSERR_NOERROR) {
        ADR_LOG("waveOutWrite failed");
      }
    }
  }


  MMAudioDevice::~MMAudioDevice() {
    waveOutReset(m_device);

    for (int i = 0; i < BUFFER_COUNT; ++i) {
      WAVEHDR& wh = m_buffers[i];
      if (wh.dwFlags & WHDR_PREPARED || wh.dwFlags & WHDR_DONE) {
        waveOutUnprepareHeader(m_device, &wh, sizeof(wh));
      }
    }

    waveOutClose(m_device);
  }


  void
  MMAudioDevice::update() {
    // if a buffer is done playing, add it to the queue again
    for (int i = 0; i < BUFFER_COUNT; ++i) {
      WAVEHDR& wh = m_buffers[i];
      if (wh.dwFlags & WHDR_DONE) {

        // unprepare
        MMRESULT result = waveOutUnprepareHeader(m_device, &wh, sizeof(wh));
        if (result != MMSYSERR_NOERROR) {
          ADR_LOG("waveOutUnprepareHeader failed");
        }

        // fill with new samples
        m_mixer->read(BUFFER_LENGTH / 4, wh.lpData);
        wh.dwFlags = 0;

        // prepare
        result = waveOutPrepareHeader(m_device, &wh, sizeof(wh));
        if (result != MMSYSERR_NOERROR) {
          ADR_LOG("waveOutPrepareHeader failed");
        }

        // write
        result = waveOutWrite(m_device, &wh, sizeof(wh));
        if (result != MMSYSERR_NOERROR) {
          ADR_LOG("waveOutWrite failed");
        }
      }
    }
    Sleep(10);
  }


  OutputStream*
  MMAudioDevice::openStream(SampleSource* source) {
    return (source ? new MMOutputStream(this, source) : 0);
  }


  OutputStream*
  MMAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, sample_count,
      channel_count, sample_rate, sample_format));
  }


  MMOutputStream::MMOutputStream(MMAudioDevice* device, SampleSource* source) {
    m_device = device;
    m_source = source;

    getMixer().addSource(m_source);
  }

  
  MMOutputStream::~MMOutputStream() {
    getMixer().removeSource(m_source);
  }


  void
  MMOutputStream::play() {
    getMixer().setPlaying(m_source, true);
  }


  void
  MMOutputStream::stop() {
    getMixer().setPlaying(m_source, false);
  }


  bool
  MMOutputStream::isPlaying() {
    return getMixer().isPlaying(m_source);
  }


  void
  MMOutputStream::reset() {
    getMixer().resetSource(m_source);
  }


  void
  MMOutputStream::setRepeat(bool repeat) {
    getMixer().setRepeat(m_source, repeat);
  }


  bool
  MMOutputStream::getRepeat() {
    return getMixer().getRepeat(m_source);
  }


  void
  MMOutputStream::setVolume(float volume) {
    getMixer().setVolume(m_source, volume);
  }


  float
  MMOutputStream::getVolume() {
    return getMixer().getVolume(m_source);
  }


  void
  MMOutputStream::setPan(float pan) {
    getMixer().setPan(m_source, pan);
  }


  float
  MMOutputStream::getPan() {
    return getMixer().getPan(m_source);
  }


  bool
  MMOutputStream::isSeekable() {
    /// @todo  implement MMOutputStream::isSeekable
    return false;
  }


  int
  MMOutputStream::getLength() {
    /// @todo  implement MMOutputStream::getLength
    return 0;
  }


  void
  MMOutputStream::setPosition(int position) {
    /// @todo  implement MMOutputStream::setPosition
  }


  int
  MMOutputStream::getPosition() {
    /// @todo  implement MMOutputStream::getPosition
    return 0;
  }


  Mixer&
  MMOutputStream::getMixer() {
    return *(m_device->m_mixer);
  }


}
