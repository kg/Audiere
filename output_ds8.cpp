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


#include "output_ds8.hpp"
#include "ds_utility.hpp"


////////////////////////////////////////////////////////////////////////////////

DS8OutputContext::DS8OutputContext()
{
  m_DirectSound     = NULL;
  m_BufferLength    = DS_DefaultBufferLength;
  m_AnonymousWindow = NULL;
}

////////////////////////////////////////////////////////////////////////////////

DS8OutputContext::~DS8OutputContext()
{
  // if the anonymous window is open, close it
  if (m_AnonymousWindow) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
  }

  // close any open streams
  StreamList::iterator i = m_OpenStreams.begin();
  while (i != m_OpenStreams.end()) {
    delete (*i);
    ++i;
  }
  m_OpenStreams.clear();

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
DS8OutputContext::Initialize(const char* parameters)
{
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

  // create the DirectSound object
  rv = CoCreateInstance(
    CLSID_DirectSound8,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IDirectSound8,
    (void**)&m_DirectSound
  );
  if (FAILED(rv) || !m_DirectSound) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    return false;
  }

  // initialize the DS8 device
  rv = m_DirectSound->Initialize(NULL);
  if (FAILED(rv)) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    m_DirectSound->Release();
    m_DirectSound = NULL;
    return false;
  }

  // set the cooperative level
  rv = m_DirectSound->SetCooperativeLevel(m_AnonymousWindow, DSSCL_NORMAL);
  if (FAILED(rv)) {
    DestroyWindow(m_AnonymousWindow);
    m_AnonymousWindow = NULL;
    m_DirectSound->Release();
    m_DirectSound = NULL;
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
DS8OutputContext::Update()
{
  // enumerate all open streams
  StreamList::iterator i = m_OpenStreams.begin();
  while (i != m_OpenStreams.end()) {
    DS8OutputStream* s = *i++;
    s->Update();
  }
}

////////////////////////////////////////////////////////////////////////////////

IOutputStream*
DS8OutputContext::OpenStream(
  int channel_count,
  int sample_rate,
  int bits_per_sample,
  ADR_SAMPLE_SOURCE source,
  ADR_SAMPLE_RESET reset,
  void* opaque)
{
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
  DSBUFFERDESC dsbd;
  memset(&dsbd, 0, sizeof(dsbd));
  dsbd.dwSize          = sizeof(dsbd);
  dsbd.dwFlags         = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
                         DSBCAPS_GLOBALFOCUS;
  dsbd.dwBufferBytes   = sample_size * buffer_length;
  dsbd.lpwfxFormat     = &wfx;
  dsbd.guid3DAlgorithm = GUID_NULL;

  // create the DirectSound buffer
  IDirectSoundBuffer* buffer;
  HRESULT result = m_DirectSound->CreateSoundBuffer(&dsbd, &buffer, NULL);
  if (FAILED(result) || !buffer) {
    return NULL;
  }

  DS8OutputStream* stream = new DS8OutputStream(
    this,
    buffer,
    sample_size,
    buffer_length,
    source,
    reset,
    opaque
  );

  // add ourselves to the list of streams and return
  m_OpenStreams.push_back(stream);
  return stream;
}

////////////////////////////////////////////////////////////////////////////////

DS8OutputStream::DS8OutputStream(
  DS8OutputContext* context,
  IDirectSoundBuffer* buffer,
  int sample_size,
  int buffer_length,
  ADR_SAMPLE_SOURCE source,
  ADR_SAMPLE_RESET reset,
  void* opaque)
{
  m_Context = context;
  m_Buffer = buffer;
  m_NextRead = 0;
  m_BufferLength = buffer_length;
  
  m_Source = source;
  m_Reset  = reset;
  m_Opaque = opaque;

  m_SampleSize = sample_size;
  m_LastSample = new BYTE[m_SampleSize];

  SetVolume(ADR_VOLUME_MAX);

  // fill the buffer with data
  FillStream();
}

////////////////////////////////////////////////////////////////////////////////

DS8OutputStream::~DS8OutputStream()
{
  // remove ourself from the list
  DS8OutputContext::StreamList::iterator i = m_Context->m_OpenStreams.begin();
  while (i != m_Context->m_OpenStreams.end()) {
    if (*i == this) {
      m_Context->m_OpenStreams.erase(i);
      break;
    }
    ++i;
  }

  // destroy the sound buffer interface
  m_Buffer->Release();
  delete[] m_LastSample;
}

////////////////////////////////////////////////////////////////////////////////

void
DS8OutputStream::FillStream()
{
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
    return;
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
DS8OutputStream::Update()
{
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
    return;
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
    return;
  }

  // now actually read samples
  int length1 = buffer1_length / m_SampleSize;
  int length2 = buffer2_length / m_SampleSize;
  int read1 = StreamRead(length1, buffer1);
  int read2 = 0;
  if (length1 == read1) {
    read2 = StreamRead(length2, buffer2);
  }

  int old_next_read = m_NextRead;
  m_NextRead = (m_NextRead + read1 + read2) % m_BufferLength;

//  char buf[1024];
//  sprintf(buf, "rl1 (%d/%d) rl2 (%d/%d)  old/new pos (%d/%d)\n",
//          read1, length1, read2, length2,
//          old_next_read, m_NextRead);
//  OutputDebugString(buf);

  // unlock
  m_Buffer->Unlock(buffer1, buffer1_length, buffer2, buffer2_length);

  
  // Should we stop?  If we didn't read any data, and if the read cursor
  // is between the play and the write cursors, we should.
  bool should_stop = (read1 + read2 == 0 &&
                      IsBetween(m_NextRead, play, write));

  if (should_stop) {

    m_Buffer->Stop();
    m_Buffer->SetCurrentPosition(0);

    m_Reset(m_Opaque);

    m_NextRead = 0;
    FillStream();

    return;
  }
}

////////////////////////////////////////////////////////////////////////////////

// read as much as possible from the stream source, fill the rest with 0
int
DS8OutputStream::StreamRead(int sample_count, void* samples)
{
  // try to read from the stream
  int samples_read = m_Source(m_Opaque, sample_count, samples);

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
DS8OutputStream::IsBetween(int position, int start, int end)
{
  if (start < end) {
    return (position >= start && position < end);
  } else {
    return (position >= start || position < end);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
DS8OutputStream::Play()
{
  m_Buffer->Play(0, 0, DSBPLAY_LOOPING);
}

////////////////////////////////////////////////////////////////////////////////

void
DS8OutputStream::Stop()
{
  m_Buffer->Stop();
}

////////////////////////////////////////////////////////////////////////////////

void
DS8OutputStream::Reset()
{
  // figure out if we're playing or not
  bool is_playing = false;
  DWORD status;
  HRESULT rv = m_Buffer->GetStatus(&status);
  if (SUCCEEDED(rv) && status & DSBSTATUS_PLAYING) {
    is_playing = true;
  }

  // if we're playing, stop
  if (is_playing) {
    m_Buffer->Stop();
  }


  m_Buffer->SetCurrentPosition(0);

  m_Reset(m_Opaque);

  m_NextRead = 0;
  FillStream();


  // if we were playing, restart
  if (is_playing) {
    m_Buffer->Play(0, 0, DSBPLAY_LOOPING);
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
DS8OutputStream::IsPlaying()
{
  DWORD status;
  HRESULT rv = m_Buffer->GetStatus(&status);
  return (SUCCEEDED(rv) && status & DSBSTATUS_PLAYING);
}

////////////////////////////////////////////////////////////////////////////////

void
DS8OutputStream::SetVolume(int volume)
{
  if (volume < ADR_VOLUME_MIN) {
    volume = ADR_VOLUME_MIN;
  } else if (volume > ADR_VOLUME_MAX) {
    volume = ADR_VOLUME_MAX;
  }

  m_Volume = volume;
  m_Buffer->SetVolume(Volume_AudiereToDirectSound(volume));
}

////////////////////////////////////////////////////////////////////////////////

int
DS8OutputStream::GetVolume()
{
  return m_Volume;
}

////////////////////////////////////////////////////////////////////////////////
