#include <math.h>
#include "device_ds.h"
#include "buffer_stream.h"
#include "debug.h"
#include "utility.h"


namespace audiere {

  static const int DEFAULT_BUFFER_LENGTH = 1000;  // one second


  DSAudioDevice*
  DSAudioDevice::create(const ParameterList& parameters) {
    ADR_GUARD("DSAudioDevice::create");

    int buffer_length = atoi(parameters.getValue("buffer", "").c_str());
    if (buffer_length == 0) {
      buffer_length = DEFAULT_BUFFER_LENGTH;
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
    SYNCHRONIZED(this);

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
    if (!source) {
      return 0;
    }

    ADR_GUARD("DSAudioDevice::openStream");
    SYNCHRONIZED(this);

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


  int
  DSAudioDevice::Volume_AudiereToDirectSound(float volume) {
    // The proper math doesn't sound right at all, so here is something that
    // sounds about right.
    double attenuate = pow(1 - volume, 3);
    return int(-10000 * attenuate);
  }


  int
  DSAudioDevice::Pan_AudiereToDirectSound(float pan) {
    if (pan < 0) {
      return -Pan_AudiereToDirectSound(-pan);
    } else {
      return -Volume_AudiereToDirectSound(1 - pan);
    }
  }

}
