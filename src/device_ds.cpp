#include <math.h>
#include "device_ds.h"
#include "buffer_stream.h"
#include "debug.h"
#include "utility.h"


namespace audiere {

  static const int DS_DefaultBufferLength = 1000;  // one second


  // DirectSound treats volumes as decibels (exponential growth like the Richter
  // scale).  We want a linear ramp.  Do the conversion!
  inline int Volume_AudiereToDirectSound(float volume) {
    // I can't figure out the proper math, and this comes close enough...
    double attenuate = pow(1 - volume, 3);
    return int(-10000 * attenuate);
  }


  inline int Pan_AudiereToDirectSound(float pan) {
    if (pan < 0) {
      return -Pan_AudiereToDirectSound(-pan);
    } else {
      return -Volume_AudiereToDirectSound(1 - pan);
    }
  }



  DSAudioDevice*
  DSAudioDevice::create(ParameterList& parameters) {
    ADR_GUARD("DSAudioDevice::create");

    int buffer_length = atoi(parameters.getValue("buffer", "").c_str());
    if (buffer_length == 0) {
      buffer_length = DS_DefaultBufferLength;
    }

    // initialize COM
    HRESULT rv = CoInitialize(NULL);
    if (FAILED(rv)) {
      return 0;
    }

    ADR_LOG("COM initialized properly");

    // register anonymous window class
    // don't worry about failure, if it fails, the window creation will fail
    WNDCLASS wc;
    wc.style          = 0;
    wc.lpfnWndProc    = DefWindowProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = GetModuleHandle(NULL);
    wc.hIcon          = NULL;
    wc.hCursor        = NULL;
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "AudiereHiddenWindow";
    RegisterClass(&wc);

    // create anonymous window
    HWND anonymous_window = CreateWindow(
      "AudiereHiddenWindow", "", WS_POPUP,
      0, 0, 0, 0,
      NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!anonymous_window) {
      return false;
    }

    ADR_LOG("Anonymous window created successfully");

    // create the DirectSound object
    IDirectSound* direct_sound;
    rv = CoCreateInstance(
      CLSID_DirectSound8,
      NULL,
      CLSCTX_INPROC_SERVER,
      IID_IDirectSound,
      (void**)&direct_sound);
    if (FAILED(rv) || !direct_sound) {
      DestroyWindow(anonymous_window);
      return 0;
    }

    ADR_LOG("Created DS object");

    // initialize the DirectSound device
    rv = direct_sound->Initialize(NULL);
    if (FAILED(rv)) {
      DestroyWindow(anonymous_window);
      direct_sound->Release();
      return 0;
    }

    ADR_LOG("Initialized DS object");

    // set the cooperative level
    rv = direct_sound->SetCooperativeLevel(anonymous_window, DSSCL_NORMAL);
    if (FAILED(rv)) {
      DestroyWindow(anonymous_window);
      direct_sound->Release();
      return 0;
    }

    ADR_LOG("Set cooperative level");

    return new DSAudioDevice(buffer_length, anonymous_window, direct_sound);
  }


  DSAudioDevice::DSAudioDevice(
    int buffer_length,
    HWND anonymous_window,
    IDirectSound* direct_sound)
  {
    m_buffer_length    = buffer_length;
    m_anonymous_window = anonymous_window;
    m_direct_sound     = direct_sound;
  }


  DSAudioDevice::~DSAudioDevice() {
    ADR_ASSERT(m_open_streams.size() == 0,
      "DirectSound output context should not die with open streams");

    // if the anonymous window is open, close it
    if (m_anonymous_window) {
      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
    }

    // shut down DirectSound
    if (m_direct_sound) {
      m_direct_sound->Release();
      m_direct_sound = NULL;
    }
  }


  void
  DSAudioDevice::update() {
    ADR_GUARD("DSAudioDevice::update");

    Lock l__(this);

    // enumerate all open streams
    StreamList::iterator i = m_open_streams.begin();
    while (i != m_open_streams.end()) {
      DSOutputStream* s = *i++;
      s->update();
    }

    Sleep(50);
  }


  OutputStream*
  DSAudioDevice::openStream(SampleSource* source) {
    ADR_GUARD("DSAudioDevice::openStream");

    Lock l__(this);

    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    int sample_size = channel_count * GetSampleSize(sample_format);

    // calculate an ideal buffer size
    int buffer_length = sample_rate * m_buffer_length / 1000;

    // define the wave format
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = channel_count;
    wfx.nSamplesPerSec  = sample_rate;
    wfx.nAvgBytesPerSec = sample_rate * sample_size;
    wfx.nBlockAlign     = sample_size;
    wfx.wBitsPerSample  = GetSampleSize(sample_format) * 8;
    wfx.cbSize          = sizeof(wfx);

    // define the DirectSound buffer type
    #if DIRECTSOUND_VERSION >= 0x0700
      DSBUFFERDESC1 dsbd;
    #else
      DSBUFFERDESC dsbd;
    #endif
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize        = sizeof(dsbd);
    dsbd.dwFlags       = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
                         DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes = sample_size * buffer_length;
    dsbd.lpwfxFormat   = &wfx;

    // create the DirectSound buffer
    IDirectSoundBuffer* buffer;
    HRESULT result = m_direct_sound->CreateSoundBuffer(
      (DSBUFFERDESC*)&dsbd,
      &buffer,
      NULL);
    if (FAILED(result) || !buffer) {
      return 0;
    }

    ADR_LOG("CreateSoundBuffer succeeded");

    DSOutputStream* stream = new DSOutputStream(
      this, buffer, buffer_length, source);

    // add ourselves to the list of streams and return
    m_open_streams.push_back(stream);
    return stream;
  }


  OutputStream*
  DSAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, sample_count,
      channel_count, sample_rate, sample_format));
  }


  void
  DSAudioDevice::removeStream(DSOutputStream* stream) {
    Lock l__(this);

    m_open_streams.remove(stream);
  }

  
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

    m_source = new RepeatableStream(source, false);

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

    delete m_source;
  }

  
  void
  DSOutputStream::play() {
    ADR_GUARD("DSOutputStream::play");
    m_buffer->Play(0, 0, DSBPLAY_LOOPING);
  }


  void
  DSOutputStream::stop() {
    ADR_GUARD("DSOutputStream::stop");
    m_buffer->Stop();
  }


  void
  DSOutputStream::reset() {
    ADR_GUARD("DSOutputStream::reset");

    // figure out if we're playing or not
    bool is_playing = isPlaying();

    // if we're playing, stop
    if (is_playing) {
      m_buffer->Stop();
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
      m_buffer->Play(0, 0, DSBPLAY_LOOPING);
    }
  }


  bool
  DSOutputStream::isPlaying() {
    DWORD status;
    HRESULT rv = m_buffer->GetStatus(&status);
    return (SUCCEEDED(rv) && status & DSBSTATUS_PLAYING);
  }


  void
  DSOutputStream::setRepeat(bool repeat) {
    SYNCHRONIZED(m_device.get());
    m_source->setRepeat(repeat);
  }


  bool
  DSOutputStream::getRepeat() {
    SYNCHRONIZED(m_device.get());
    return m_source->getRepeat();
  }


  void
  DSOutputStream::setVolume(float volume) {
    m_volume = volume;
    m_buffer->SetVolume(Volume_AudiereToDirectSound(volume));
  }


  float
  DSOutputStream::getVolume() {
    return m_volume;
  }


  void
  DSOutputStream::setPan(float pan) {
    m_pan = pan;
    m_buffer->SetPan(Pan_AudiereToDirectSound(pan));
  }


  float
  DSOutputStream::getPan() {
    return m_pan;
  }


  bool
  DSOutputStream::isSeekable() {
    return m_source->isSeekable();
  }


  int
  DSOutputStream::getLength() {
    return m_source->getLength();
  }


  void
  DSOutputStream::setPosition(int position) {
    SYNCHRONIZED(m_device.get());
    m_source->setPosition(position);
    reset();
  }


  int
  DSOutputStream::getPosition() {
    /// @todo  use play cursors and stuff... this isn't accurate information
    return m_source->getPosition();
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

      m_buffer->Stop();
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
