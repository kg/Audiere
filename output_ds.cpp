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
#include "output_ds.hpp"
#include "debug.hpp"


static const int DS_DefaultBufferLength = 1000;  // one second


// DirectSound treats volumes as decibels (exponential growth like the Richter
// scale).  We want a linear ramp.  Do the conversion!
inline int Volume_AudiereToDirectSound(int volume) {
  // I can't figure out the proper math, and this comes close enough...
  double fv = volume / 255.0;  // range: 0-1
  double attenuate = pow(1 - fv, 3);
  return int(-10000 * attenuate);
}


////////////////////////////////////////////////////////////////////////////////

DSOutputContext::DSOutputContext()
{
  m_DirectSound     = NULL;
  m_BufferLength    = DS_DefaultBufferLength;
  m_AnonymousWindow = NULL;
}

////////////////////////////////////////////////////////////////////////////////

DSOutputContext::~DSOutputContext()
{
  ADR_ASSERT(m_OpenStreams.size() == 0,
    "DirectSound output context should not die with open streams");

  // if the anonymous window is open, close it
  if (m_AnonymousWindow) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
  }

  // shut down DirectSound
  if (m_DirectSound) {
    m_DirectSound->Release();
    m_DirectSound = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

static LRESULT CALLBACK WindowProc(
  HWND window,
  UINT message,
  WPARAM wparam,
  LPARAM lparam)
{
  return DefWindowProc(window, message, wparam, lparam);
}

bool
DSOutputContext::Initialize(const char* parameters)
{
  ADR_GUARD("DSOutputContext::Initialize");

  // parse the parameter list
  ParameterList pl;
  ParseParameters(parameters, pl);

  ParameterList::iterator i = pl.begin();
  while (i != pl.end()) {
    
    if (i->first.c_str() == "buffer") {
      m_BufferLength = atoi(i->second.c_str());
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
  wc.lpfnWndProc    = WindowProc;
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
  m_AnonymousWindow = CreateWindow(
    "AudiereHiddenWindow", "", WS_POPUP,
    0, 0, 0, 0,
    NULL, NULL, GetModuleHandle(NULL), NULL);
  if (!m_AnonymousWindow) {
    return false;
  }

  ADR_LOG("Anonymous window created successfully");

  // create the DirectSound object
  rv = CoCreateInstance(
    GetCLSID(),
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IDirectSound,
    (void**)&m_DirectSound);
  if (FAILED(rv) || !m_DirectSound) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    return false;
  }

  ADR_LOG("Created DS object");

  // initialize the DirectSound device
  rv = m_DirectSound->Initialize(NULL);
  if (FAILED(rv)) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    m_DirectSound->Release();
    m_DirectSound = NULL;
    return false;
  }

  ADR_LOG("Initialized DS object");

  // set the cooperative level
  rv = m_DirectSound->SetCooperativeLevel(
    m_AnonymousWindow,
    GetCooperativeLevel());
  if (FAILED(rv)) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    m_DirectSound->Release();
    m_DirectSound = NULL;
    return false;
  }

  ADR_LOG("Set cooperative level");

  if (!CreatePrimarySoundBuffer(m_DirectSound)) {

    ADR_LOG("CreatePrimarySoundBuffer failed");

    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    m_DirectSound->Release();
    m_DirectSound = NULL;
    return false;
  }

  ADR_LOG("Primary sound buffer created");

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputContext::Update()
{
  ADR_GUARD("DSOutputContext::Update");

  // enumerate all open streams
  StreamList::iterator i = m_OpenStreams.begin();
  while (i != m_OpenStreams.end()) {
    DSOutputStream* s = *i++;
    s->Update();
  }

  Sleep(50);
}

////////////////////////////////////////////////////////////////////////////////

IOutputStream*
DSOutputContext::OpenStream(ISampleSource* source)
{
  ADR_GUARD("DSOutputContext::OpenStream");

  int channel_count, sample_rate, bits_per_sample;
  source->GetFormat(channel_count, sample_rate, bits_per_sample);

  int sample_size = channel_count * bits_per_sample / 8;

  // calculate an ideal buffer size
  int buffer_length = sample_rate * m_BufferLength / 1000;

  // define the wave format
  WAVEFORMATEX wfx;
  memset(&wfx, 0, sizeof(wfx));
  wfx.wFormatTag      = WAVE_FORMAT_PCM;
  wfx.nChannels       = channel_count;
  wfx.nSamplesPerSec  = sample_rate;
  wfx.nAvgBytesPerSec = sample_rate * sample_size;
  wfx.nBlockAlign     = sample_size;
  wfx.wBitsPerSample  = bits_per_sample;
  wfx.cbSize          = sizeof(wfx);

  // define the DirectSound buffer type
  #ifdef USE_DIRECTX8
    DSBUFFERDESC dsbd;
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
  HRESULT result = m_DirectSound->CreateSoundBuffer(&dsbd, &buffer, NULL);
  if (FAILED(result) || !buffer) {
    return 0;
  }

  ADR_LOG("CreateSoundBuffer succeeded");

  DSOutputStream* stream = new DSOutputStream(
    this,
    buffer,
    sample_size,
    buffer_length,
    source);

  // add ourselves to the list of streams and return
  m_OpenStreams.push_back(stream);

  return stream;
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputContext::RemoveStream(DSOutputStream* stream)
{
  m_OpenStreams.remove(stream);
}

////////////////////////////////////////////////////////////////////////////////

DSOutputStream::DSOutputStream(
  DSOutputContext* context,
  IDirectSoundBuffer* buffer,
  int sample_size,
  int buffer_length,
  ISampleSource* source)
{
  ADR_GUARD("DSOutputStream::DSOutputStream");

  m_Context = context;
  m_Buffer = buffer;
  m_NextRead = 0;
  m_BufferLength = buffer_length;

  m_Source = source;

  m_SampleSize = sample_size;
  m_LastSample = new BYTE[sample_size];

  SetVolume(ADR_VOLUME_MAX);

  // fill the buffer with data
  FillStream();
}

////////////////////////////////////////////////////////////////////////////////

DSOutputStream::~DSOutputStream()
{
  ADR_GUARD("DSOutputStream::~DSOutputStream");

  m_Context->RemoveStream(this);

  // destroy the sound buffer interface
  m_Buffer->Release();
  delete[] m_LastSample;
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputStream::FillStream()
{
  ADR_GUARD("DSOutputStream::FillStream");

  // we know the stream is stopped, so just lock the buffer and fill it

  void* buffer = NULL;
  DWORD buffer_length = 0;

  // lock
  HRESULT result = m_Buffer->Lock(
    0,
    m_BufferLength * m_SampleSize,
    &buffer,
    &buffer_length,
    NULL,
    NULL,
    0
  );
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
  int samples_to_read = buffer_length / m_SampleSize;
  int samples_read = StreamRead(samples_to_read, buffer);
  if (samples_read != samples_to_read) {
    m_NextRead = samples_read;
  }

  // unlock
  m_Buffer->Unlock(buffer, buffer_length, NULL, 0);
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputStream::Update()
{
  ADR_GUARD("DSOutputStream::Update");

  // if it's not playing, don't do anything
  if (!IsPlaying()) {
    return;
  }

  /* this method reads more PCM data into the stream if it is required */

  // read the stream's play and write cursors
  DWORD play;
  DWORD write;
  HRESULT result = m_Buffer->GetCurrentPosition(&play, &write);
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
  play  /= m_SampleSize;
  write /= m_SampleSize;

  // read from |m_NextRead| to |play|
  int read_length = play - m_NextRead;
  if (read_length < 0) {
    read_length += m_BufferLength;
  }

  if (read_length == 0) {
    return;
  }

  // lock the buffer
  void* buffer1;
  void* buffer2;
  DWORD buffer1_length;
  DWORD buffer2_length;
  result = m_Buffer->Lock(
    m_NextRead * m_SampleSize,
    read_length * m_SampleSize,
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
  int length1 = buffer1_length / m_SampleSize;
  int length2 = buffer2_length / m_SampleSize;
  int read = StreamRead(length1, buffer1);
  if (length1 == read) {
    read += StreamRead(length2, buffer2);
  }

  ADR_IF_DEBUG {
    char str[80];
    sprintf(str, "read: %d", read);
    ADR_LOG(str);
  }

  m_NextRead = (m_NextRead + read) % m_BufferLength;

  // unlock
  m_Buffer->Unlock(buffer1, buffer1_length, buffer2, buffer2_length);

  
  // Should we stop?  If we didn't read any data, and if the read cursor
  // is between the play and the write cursors, we should.
  if (read == 0 && IsBetween(m_NextRead, play, write)) {

    ADR_LOG("Stopping stream!");

    m_Buffer->Stop();
    m_Buffer->SetCurrentPosition(0);

    m_Source->Reset();

    m_NextRead = 0;
    FillStream();

    return;
  }
}

////////////////////////////////////////////////////////////////////////////////

// read as much as possible from the stream source, fill the rest with 0
int
DSOutputStream::StreamRead(int sample_count, void* samples)
{
  ADR_GUARD("StreamRead");

  // try to read from the stream
  int samples_read = m_Source->Read(sample_count, samples);

  // read the last sample
  if (samples_read > 0) {
    memcpy(
      m_LastSample,
      (BYTE*)samples + (samples_read - 1) * m_SampleSize,
      m_SampleSize
    );
  }

  // fill the rest with silence
  BYTE* out = (BYTE*)samples + m_SampleSize * samples_read;
  int c = sample_count - samples_read;
  while (c--) {
    memcpy(out, m_LastSample, m_SampleSize);
    out += m_SampleSize;
  }

  return samples_read;
}

////////////////////////////////////////////////////////////////////////////////

bool
DSOutputStream::IsBetween(int position, int start, int end)
{
  if (start < end) {
    return (position >= start && position < end);
  } else {
    return (position >= start || position < end);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputStream::Play()
{
  m_Buffer->Play(0, 0, DSBPLAY_LOOPING);
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputStream::Stop()
{
  m_Buffer->Stop();
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputStream::Reset()
{
  ADR_GUARD("DSOutputStream::Reset");

  // figure out if we're playing or not
  bool is_playing = IsPlaying();

  // if we're playing, stop
  if (is_playing) {
    m_Buffer->Stop();
  }

  m_Buffer->SetCurrentPosition(0);
  m_Source->Reset();
  m_NextRead = 0;
  FillStream();

  // if we were playing, restart
  if (is_playing) {
    m_Buffer->Play(0, 0, DSBPLAY_LOOPING);
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
DSOutputStream::IsPlaying()
{
  DWORD status;
  HRESULT rv = m_Buffer->GetStatus(&status);
  return (SUCCEEDED(rv) && status & DSBSTATUS_PLAYING);
}

////////////////////////////////////////////////////////////////////////////////

void
DSOutputStream::SetVolume(int volume)
{
  m_Volume = volume;
  m_Buffer->SetVolume(Volume_AudiereToDirectSound(volume));
}

////////////////////////////////////////////////////////////////////////////////

int
DSOutputStream::GetVolume()
{
  return m_Volume;
}

////////////////////////////////////////////////////////////////////////////////
