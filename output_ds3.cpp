#include "output_ds3.hpp"
#include "debug.hpp"


////////////////////////////////////////////////////////////////////////////////

DS3OutputContext::DS3OutputContext()
{
  m_PrimaryBuffer = 0;
}

////////////////////////////////////////////////////////////////////////////////

DS3OutputContext::~DS3OutputContext()
{
  if (m_PrimaryBuffer) {
    m_PrimaryBuffer->Release();
    m_PrimaryBuffer = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
DS3OutputContext::CreatePrimarySoundBuffer(IDirectSound* ds)
{
  ADR_GUARD("DS3OutputContext::CreatePrimarySoundBuffer");

  // create a primary sound buffer
  #ifdef USE_DIRECTX8
    DSBUFFERDESC1 dsbd;
  #else
    DSBUFFERDESC dsbd;
  #endif

  dsbd.dwSize        = sizeof(dsbd);
  dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
  dsbd.dwBufferBytes = 0;  // primary buffers don't specify size
  dsbd.dwReserved    = 0;
  dsbd.lpwfxFormat   = NULL;  // primary buffers use SetFormat()
  HRESULT rv = ds->CreateSoundBuffer(
    (DSBUFFERDESC*)&dsbd,
    &m_PrimaryBuffer,
    NULL);
  if (FAILED(rv)) {
    ADR_LOG("CreateSoundBuffer failed");
    return false;
  }

  ADR_LOG("CreateSoundBuffer succeeded");

  // set our maximum output format to 44100, 16, stereo
  // we don't really care about this
  WAVEFORMATEX wf;
  memset(&wf, 0, sizeof(wf));
  wf.wFormatTag = WAVE_FORMAT_PCM; 
  wf.nChannels = 2; 
  wf.nSamplesPerSec = 44100; 
  wf.wBitsPerSample = 16; 
  wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
  wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

  rv = m_PrimaryBuffer->SetFormat(&wf); 
  if (FAILED(rv)) {
    return false;
  }

  ADR_LOG("SetFormat succeeded");

  return true;
}

////////////////////////////////////////////////////////////////////////////////
