/*
  Audiere DirectSound driver

  Terminology

  block 
  - set of bytes for the smallest block of audio in a stream
    (including all of the channels)

  buffer
  - a circular audio buffer which is constantly refilled with
    new data from the stream source

  segment
  - buffers are split into a set of segments data flows from
    the stream source into the buffer a segment at a time

*/


#include <math.h>
#include "device_ds.h"
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
      return Volume_AudiereToDirectSound(1 - pan);
    }
  }



  DSAudioDevice::DSAudioDevice() {
    m_direct_sound     = NULL;
    m_buffer_length    = DS_DefaultBufferLength;
    m_anonymous_window = NULL;
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


  bool
  DSAudioDevice::initialize(const char* parameters) {
    ADR_GUARD("DSAudioDevice::initialize");

    // parse the parameter list
    ParameterList pl(ParseParameters(parameters));

    ParameterList::iterator i = pl.begin();
    while (i != pl.end()) {
    
      if (i->first.c_str() == "buffer") {
        m_buffer_length = atoi(i->second.c_str());
        if (m_buffer_length == 0) {
          m_buffer_length = DS_DefaultBufferLength;
        }
      }

      ++i;
    }

    // initialize COM
    HRESULT rv = CoInitialize(NULL);
    if (FAILED(rv)) {
      return false;
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
    m_anonymous_window = CreateWindow(
      "AudiereHiddenWindow", "", WS_POPUP,
      0, 0, 0, 0,
      NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!m_anonymous_window) {
      return false;
    }

    ADR_LOG("Anonymous window created successfully");

    // create the DirectSound object
    rv = CoCreateInstance(
      getCLSID(),
      NULL,
      CLSCTX_INPROC_SERVER,
      IID_IDirectSound,
      (void**)&m_direct_sound);
    if (FAILED(rv) || !m_direct_sound) {
      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
      return false;
    }

    ADR_LOG("Created DS object");

    // initialize the DirectSound device
    rv = m_direct_sound->Initialize(NULL);
    if (FAILED(rv)) {
      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
      m_direct_sound->Release();
      m_direct_sound = NULL;
      return false;
    }

    ADR_LOG("Initialized DS object");

    // set the cooperative level
    rv = m_direct_sound->SetCooperativeLevel(
      m_anonymous_window,
      getCooperativeLevel());
    if (FAILED(rv)) {
      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
      m_direct_sound->Release();
      m_direct_sound = NULL;
      return false;
    }

    ADR_LOG("Set cooperative level");

    if (!createPrimarySoundBuffer(m_direct_sound)) {

      ADR_LOG("CreatePrimarySoundBuffer failed");

      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
      m_direct_sound->Release();
      m_direct_sound = NULL;
      return false;
    }

    ADR_LOG("Primary sound buffer created");

    return true;
  }


  void
  DSAudioDevice::update() {
    ADR_GUARD("DSAudioDevice::Update");

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
    ADR_GUARD("DSAudioDevice::OpenStream");

    Lock l__(this);

    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    int sample_size = channel_count * GetBytesPerSample(sample_format);

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
    wfx.wBitsPerSample  = GetBytesPerSample(sample_format) * 8;
    wfx.cbSize          = sizeof(wfx);

    // define the DirectSound buffer type
    #if DIRECTSOUND_VERSION >= 0x0700
      DSBUFFERDESC1 dsbd;
    #else
      DSBUFFERDESC dsbd;
    #endif
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize          = sizeof(dsbd);
    dsbd.dwFlags         = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
                           DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes   = sample_size * buffer_length;
    dsbd.lpwfxFormat     = &wfx;

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
      this, buffer, sample_size, buffer_length, source);

    // add ourselves to the list of streams and return
    m_open_streams.push_back(stream);
    return stream;
  }


  void
  DSAudioDevice::removeStream(DSOutputStream* stream) {
    Lock l__(this);

    m_open_streams.remove(stream);
  }

  
  DSOutputStream::DSOutputStream(
    DSAudioDevice* device,
    IDirectSoundBuffer* buffer,
    int sample_size,
    int buffer_length,
    SampleSource* source)
  {
    ADR_GUARD("DSOutputStream::DSOutputStream");

    m_device = device;
    m_device->ref();

    m_buffer        = buffer;
    m_next_read     = 0;
    m_buffer_length = buffer_length;

    m_source = source;

    m_sample_size = sample_size;
    m_last_sample = new BYTE[sample_size];

    setVolume(1);

    // fill the buffer with data
    fillStream();
  }


  DSOutputStream::~DSOutputStream() {
    ADR_GUARD("DSOutputStream::~DSOutputStream");

    m_device->removeStream(this);
    m_device->unref();

    // destroy the sound buffer interface
    m_buffer->Release();
    delete[] m_last_sample;

    delete m_source;
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
    ADR_GUARD("DSOutputStream::Update");

    // if it's not playing, don't do anything
    if (!isPlaying()) {
      return;
    }

    /* this method reads more PCM data into the stream if it is required */

    // read the stream's play and write cursors
    DWORD play;
    DWORD write;
    HRESULT result = m_buffer->GetCurrentPosition(&play, &write);
    if (FAILED(result)) {
      ADR_LOG("GetCurrentPosition failed");
      return;
    }

    ADR_IF_DEBUG {
      char str[160];
      sprintf(str, "play: %d  write: %d", play, write);
      ADR_LOG(str);
    }

    // deal with them in samples, not bytes
    play  /= m_sample_size;
    write /= m_sample_size;

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
      &buffer1,
      &buffer1_length,
      &buffer2,
      &buffer2_length,
      0
    );
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

  
    // Should we stop?  If we didn't read any data, and if the read cursor
    // is between the play and the write cursors, we should.
    if (read == 0 && isBetween(m_next_read, play, write)) {

      ADR_LOG("Stopping stream!");

      m_buffer->Stop();
      m_buffer->SetCurrentPosition(0);

      m_source->reset();

      m_next_read = 0;
      fillStream();

      return;
    }
  }


  // read as much as possible from the stream source, fill the rest with 0
  int
  DSOutputStream::streamRead(int sample_count, void* samples) {
    ADR_GUARD("StreamRead");

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

    return samples_read;
  }


  bool
  DSOutputStream::isBetween(int position, int start, int end) {
    if (start < end) {
      return (position >= start && position < end);
    } else {
      return (position >= start || position < end);
    }
  }


  void
  DSOutputStream::play() {
    m_buffer->Play(0, 0, DSBPLAY_LOOPING);
  }


  void
  DSOutputStream::stop() {
    m_buffer->Stop();
  }


  void
  DSOutputStream::reset() {
    ADR_GUARD("DSOutputStream::Reset");

    // figure out if we're playing or not
    bool is_playing = isPlaying();

    // if we're playing, stop
    if (is_playing) {
      m_buffer->Stop();
    }

    m_buffer->SetCurrentPosition(0);
    m_source->reset();
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

}
