#include "output_ds3.hpp"


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
DS3OutputContext::CreatePrimarySoundBuffer()
{
  // create a primary sound buffer
  DSBUFFERDESC dsbd;
  memset(&dsbd, 0, sizeof(dsbd));
  dsbd.dwSize = sizeof(dsbd);
  dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
  HRESULT rv = m_DirectSound->CreateSoundBuffer(&dsbd, &m_PrimaryBuffer, NULL);
  if (FAILED(rv)) {
    return false;
  }

  // set our maximum output format to 44100, 16, stereo
  // we don't really care about this
  WAVEFORMATEX wf;
  memset(&wf, 0, sizeof(wf));
  wf.wFormatTag = WAVE_FORMAT_PCM; 
  wf.nChannels = 2; 
  wf.nSamplesPerSec = 44100; 
  wf.wBitsPerSample = 16; 
  wf.nBlockAlign = wf.wBitsPerSample / 8 * wf.nChannels;
  wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

  rv = m_PrimaryBuffer->SetFormat(&wf); 
  if (FAILED(rv)) {
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
