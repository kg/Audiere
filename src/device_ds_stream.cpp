#include "device_ds_stream.h"
#include "device_ds.h"
#include "debug.h"


namespace audiere {

  DSOutputStream::DSOutputStream(
    DSAudioDevice* device,
    IDirectSoundBuffer* buffer,
    int buffer_length,
    SampleSource* source)
  {
    ADR_GUARD("DSOutputStream::DSOutputStream");

    m_device = device;

    m_buffer        = buffer;
    m_buffer_length = buffer_length;
    m_next_read     = 0;
    m_last_play     = 0;

    DWORD frequency;
    m_buffer->GetFrequency(&frequency);
    m_base_frequency = frequency;

    m_is_playing = false;

    m_source = new RepeatableStream(source);

    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);
    m_sample_size = GetSampleSize(sample_format) * channel_count;

    m_total_read    = 0;
    m_total_written = 0;

    m_last_sample = new BYTE[m_sample_size];

    setVolume(1);
    setPan(0);

    // fill the buffer with data
    fillStream();
  }


  DSOutputStream::~DSOutputStream() {
    ADR_GUARD("DSOutputStream::~DSOutputStream");

    m_device->removeStream(this);

    // destroy the sound buffer interface
    m_buffer->Release();
    delete[] m_last_sample;
  }

  
  void
  DSOutputStream::play() {
    ADR_GUARD("DSOutputStream::play");
    m_buffer->Play(0, 0, DSBPLAY_LOOPING);
    m_is_playing = true;
  }


  void
  DSOutputStream::stop() {
    ADR_GUARD("DSOutputStream::stop");
    m_buffer->Stop();
    m_is_playing = false;
  }


  bool
  DSOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  DSOutputStream::reset() {
    ADR_GUARD("DSOutputStream::reset");
    SYNCHRONIZED(this);

    // figure out if we're playing or not
    bool is_playing = isPlaying();

    // if we're playing, stop
    if (is_playing) {
      stop();
    }

    m_buffer->SetCurrentPosition(0);
    m_last_play = 0;

    m_source->reset();
    m_total_read = 0;
    m_total_written = 0;
    m_next_read = 0;
    fillStream();

    // if we were playing, restart
    if (is_playing) {
      play();
    }
  }


  void
  DSOutputStream::setRepeat(bool repeat) {
    SYNCHRONIZED(this);
    m_source->setRepeat(repeat);
  }


  bool
  DSOutputStream::getRepeat() {
    SYNCHRONIZED(this);
    return m_source->getRepeat();
  }


  void
  DSOutputStream::setVolume(float volume) {
    m_volume = volume;
    m_buffer->SetVolume(DSAudioDevice::Volume_AudiereToDirectSound(volume));
  }


  float
  DSOutputStream::getVolume() {
    return m_volume;
  }


  void
  DSOutputStream::setPan(float pan) {
    m_pan = pan;
    m_buffer->SetPan(DSAudioDevice::Pan_AudiereToDirectSound(pan));
  }


  float
  DSOutputStream::getPan() {
    return m_pan;
  }


  void
  DSOutputStream::setPitchShift(float shift) {
    m_buffer->SetFrequency(DWORD(m_base_frequency * shift));
  }


  float
  DSOutputStream::getPitchShift() {
    DWORD frequency;
    m_buffer->GetFrequency(&frequency);
    return float(frequency) / m_base_frequency;
  }
  

  bool
  DSOutputStream::isSeekable() {
    SYNCHRONIZED(this);
    return m_source->isSeekable();
  }


  int
  DSOutputStream::getLength() {
    SYNCHRONIZED(this);
    return m_source->getLength();
  }


  void
  DSOutputStream::setPosition(int position) {
    SYNCHRONIZED(this);

    // figure out if we're playing or not
    bool is_playing = isPlaying();

    // if we're playing, stop
    if (is_playing) {
      stop();
    }

    m_source->setPosition(position);
    m_last_play = 0;

    m_source->setPosition(position);
    m_total_read = 0;
    m_total_written = 0;
    m_next_read = 0;
    fillStream();

    // if we were playing, restart
    if (is_playing) {
      play();
    }
  }


  int
  DSOutputStream::getPosition() {
    SYNCHRONIZED(this);
    int pos = m_source->getPosition() - (m_total_read - m_total_written);
    if (pos < 0) {
      pos += m_source->getLength();
    }
    return pos;
  }


  void
  DSOutputStream::fillStream() {
    ADR_GUARD("DSOutputStream::fillStream");

    // we know the stream is stopped, so just lock the buffer and fill it

    void* buffer = NULL;
    DWORD buffer_length = 0;

    // lock
    HRESULT result = m_buffer->Lock(
      0,
      m_buffer_length * m_sample_size,
      &buffer,
      &buffer_length,
      NULL,
      NULL,
      0);
    if (FAILED(result) || !buffer) {
      ADR_LOG("FillStream failed!");
      return;
    }

    ADR_IF_DEBUG {
      char str[80];
      sprintf(str, "Buffer Length = %d", buffer_length);
      ADR_LOG(str);
    }

    // fill
    int samples_to_read = buffer_length / m_sample_size;
    int samples_read = streamRead(samples_to_read, buffer);
    if (samples_read != samples_to_read) {
      m_next_read = samples_read;
    } else {
      m_next_read = 0;
    }

    // unlock
    m_buffer->Unlock(buffer, buffer_length, NULL, 0);
  }


  void
  DSOutputStream::update() {
    SYNCHRONIZED(this);

    // if it's not playing, don't do anything
    if (!isPlaying()) {
      return;
    }

    ADR_GUARD("DSOutputStream::update");

    /* this method reads more PCM data into the stream if it is required */

    // read the stream's play and write cursors
    DWORD play, write;
    HRESULT result = m_buffer->GetCurrentPosition(&play, &write);
    if (FAILED(result)) {
      ADR_LOG("GetCurrentPosition failed");
      return;
    }

    ADR_IF_DEBUG {
      char str[160];
      sprintf(str,
        "play: %d  write: %d  nextread: %d",
        play, write, m_next_read);
      ADR_LOG(str);
    }

    // deal with them in samples, not bytes
    play  /= m_sample_size;
    write /= m_sample_size;

    // how many samples have we written since the last update?
    if (play < m_last_play) {
      m_total_written += play + m_buffer_length - m_last_play;
    } else {
      m_total_written += play - m_last_play;
    }
    m_last_play = play;

    // read from |m_next_read| to |play|
    int read_length = play - m_next_read;
    if (read_length < 0) {
      read_length += m_buffer_length;
    }

    if (read_length == 0) {
      return;
    }

    // lock the buffer
    void* buffer1;
    void* buffer2;
    DWORD buffer1_length;
    DWORD buffer2_length;
    result = m_buffer->Lock(
      m_next_read * m_sample_size,
      read_length * m_sample_size,
      &buffer1, &buffer1_length,
      &buffer2, &buffer2_length,
      0);
    if (FAILED(result)) {
      ADR_LOG("Lock() failed!");
      return;
    }

    ADR_IF_DEBUG {
      char str[160];
      sprintf(str, "buffer1: %d  buffer2: %d", buffer1_length, buffer2_length);
      ADR_LOG(str);
    }

    // now actually read samples
    int length1 = buffer1_length / m_sample_size;
    int length2 = buffer2_length / m_sample_size;
    int read = streamRead(length1, buffer1);
    if (length1 == read) {
      read += streamRead(length2, buffer2);
    }

    ADR_IF_DEBUG {
      char str[80];
      sprintf(str, "read: %d", read);
      ADR_LOG(str);
    }

    m_next_read = (m_next_read + read) % m_buffer_length;

    // unlock
    m_buffer->Unlock(buffer1, buffer1_length, buffer2, buffer2_length);

  
    // Should we stop?
    if (m_total_written > m_total_read) {
      ADR_LOG("Stopping stream!");

      stop();
      m_buffer->SetCurrentPosition(0);
      m_last_play = 0;

      m_source->reset();

      m_total_written = 0;
      m_total_read = 0;
      m_next_read = 0;
      fillStream();

      return;
    }
  }


  // read as much as possible from the stream source, fill the rest with 0
  int
  DSOutputStream::streamRead(int sample_count, void* samples) {
    ADR_GUARD("streamRead");

    // try to read from the stream
    int samples_read = m_source->read(sample_count, samples);

    // read the last sample
    if (samples_read > 0) {
      memcpy(
        m_last_sample,
        (BYTE*)samples + (samples_read - 1) * m_sample_size,
        m_sample_size);
    }

    // fill the rest with silence
    BYTE* out = (BYTE*)samples + m_sample_size * samples_read;
    int c = sample_count - samples_read;
    while (c--) {
      memcpy(out, m_last_sample, m_sample_size);
      out += m_sample_size;
    }

    m_total_read += samples_read;
    return samples_read;
  }

}
