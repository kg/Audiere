#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include <functional>
#include "device_null.h"
#include "buffer_stream.h"
#include "timer.h"
#include "threads.h"
#include "utility.h"

namespace audiere {

  NullAudioDevice*
  NullAudioDevice::create(const ParameterList& /*parameters*/) {
    return new NullAudioDevice;
  }


  NullAudioDevice::NullAudioDevice() {
  }


  NullAudioDevice::~NullAudioDevice() {
    ADR_GUARD("~NullAudioDevice");

    ADR_ASSERT(m_streams.size() == 0,
      "Null output context should not die with open streams");
  }


  void
  NullAudioDevice::update() {
    ADR_GUARD("NullAudioDevice::update");

    Lock l__(this);

    /*
      UGH.  We can't use VC6 here because you can't use mem_fun with void
      functions.  :(
    std::for_each(
      m_streams.begin(),
      m_streams.end(),
      std::mem_fun(&NullOutputStream::Update));
    */

    StreamList::iterator i = m_streams.begin();
    for (; i != m_streams.end(); ++i) {
      (*i)->update();
    }

    AI_Sleep(50);
  }


  OutputStream*
  NullAudioDevice::openStream(SampleSource* source) {
    if (!source) {
      return 0;
    }

    Lock l__(this);

    NullOutputStream* stream = new NullOutputStream(this, source);
    m_streams.insert(stream);
    return stream;
  }


  OutputStream*
  NullAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, sample_count,
      channel_count, sample_rate, sample_format));
  }


  void
  NullAudioDevice::removeStream(NullOutputStream* stream) {
    Lock l__(this);

    m_streams.erase(stream);
  }


  NullOutputStream::NullOutputStream(
    NullAudioDevice* device,
    SampleSource* source)
  : m_device(device)
  , m_source(new RepeatableStream(source))
  , m_is_playing(false)
  , m_volume(1)
  , m_pan(0)
  , m_last_update(0)
  {
    m_source->getFormat(m_channel_count, m_sample_rate, m_sample_format);
  }


  NullOutputStream::~NullOutputStream() {
    m_device->removeStream(this);

    delete m_source;
  }


  void
  NullOutputStream::play() {
    ADR_GUARD("NullOutputStream::Play");
    m_is_playing = true;
    resetTimer();
  }


  void
  NullOutputStream::stop() {
    m_is_playing = false;
  }


  void
  NullOutputStream::reset() {
    resetTimer();
  }


  bool
  NullOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  NullOutputStream::setRepeat(bool repeat) {
    SYNCHRONIZED(m_device.get());
    m_source->setRepeat(repeat);
  }


  bool
  NullOutputStream::getRepeat() {
    SYNCHRONIZED(m_device.get());
    return m_source->getRepeat();
  }


  void
  NullOutputStream::setVolume(float volume) {
    m_volume = volume;
  }


  float
  NullOutputStream::getVolume() {
    return m_volume;
  }

  
  void
  NullOutputStream::setPan(float pan) {
    m_pan = pan;
  }

  
  float
  NullOutputStream::getPan() {
    return m_pan;
  }


  bool
  NullOutputStream::isSeekable() {
    return m_source->isSeekable();
  }


  int
  NullOutputStream::getLength() {
    return m_source->getLength();
  }


  void
  NullOutputStream::setPosition(int position) {
    SYNCHRONIZED(m_device.get());
    m_source->setPosition(position);
    reset();
  }


  int
  NullOutputStream::getPosition() {
    return m_source->getPosition();
  }


  void
  NullOutputStream::resetTimer() {
    m_last_update = GetNow();
  }


  void
  NullOutputStream::update() {
    ADR_GUARD("NullOutputStream::Update");

    if (m_is_playing) {
      ADR_LOG("Null output stream is playing");

      // get number of milliseconds elapsed since last playing update
      // so we can read that much time worth of samples
      u64 now = GetNow();
      u64 elapsed = now - m_last_update;

      ADR_IF_DEBUG {
        char str[100];
        sprintf(str, "Elapsed: %I64d", elapsed);
        ADR_LOG(str);
      }

      int samples_to_read = int(m_sample_rate * elapsed / 1000000);

      ADR_IF_DEBUG {
        char str[100];
        sprintf(str, "Samples to read: %d", samples_to_read);
        ADR_LOG(str);
      }

      int samples_read = dummyRead(samples_to_read);

      if (samples_read != samples_to_read) {
        ADR_LOG("Stopping null output stream");
        m_source->reset();
        stop();
      }

      m_last_update = now;
    }
  }


  int
  NullOutputStream::dummyRead(int samples_to_read) {
    int total = 0;  // number of samples read so far

    const int bytes_per_sample = GetSampleSize(m_sample_format);

    // read samples into dummy buffer, counting the number we actually read
    u8* dummy = new u8[1024 * m_channel_count * bytes_per_sample];
    while (samples_to_read > 0) {
      int read = std::min(1024, samples_to_read);
      int actual_read = m_source->read(read, dummy);
      total += actual_read;
      samples_to_read -= actual_read;
      if (actual_read < read) {
        break;
      }
    }

    delete[] dummy;
    return total;
  }

}
