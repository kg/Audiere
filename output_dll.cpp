#include <windows.h>
#include "output_dll.hpp"


////////////////////////////////////////////////////////////////////////////////

// this is useful to prevent compiler warnings
template<typename T, typename S>
void assign(T& dest, S src) {
  dest = reinterpret_cast<T>(src);
}

////////////////////////////////////////////////////////////////////////////////

DLLOutputContext::DLLOutputContext()
{
  m_DLL = NULL;

  AO_OpenDriver      = NULL;
  AO_CloseDriver     = NULL;
  AO_Update          = NULL;
  AO_OpenStream      = NULL;
  AO_CloseStream     = NULL;
  AO_PlayStream      = NULL;
  AO_StopStream      = NULL;
  AO_ResetStream     = NULL;
  AO_IsStreamPlaying = NULL;
  AO_SetVolume       = NULL;
  AO_GetVolume       = NULL;
}

////////////////////////////////////////////////////////////////////////////////

DLLOutputContext::~DLLOutputContext()
{
  if (m_DLL) {
    AO_CloseDriver();
    FreeLibrary(m_DLL);
    m_DLL = NULL;
  }

  AO_OpenDriver      = NULL;
  AO_CloseDriver     = NULL;
  AO_Update          = NULL;
  AO_OpenStream      = NULL;
  AO_CloseStream     = NULL;
  AO_PlayStream      = NULL;
  AO_StopStream      = NULL;
  AO_ResetStream     = NULL;
  AO_IsStreamPlaying = NULL;
  AO_SetVolume       = NULL;
  AO_GetVolume       = NULL;
}

////////////////////////////////////////////////////////////////////////////////

bool
DLLOutputContext::Initialize(const char* parameters)
{
  // parse the parameter list
  ParameterList pl;
  ParseParameters(parameters, pl);

  std::string dll_name;

  ParameterList::iterator i = pl.begin();
  while (i != pl.end()) {
    
    if (strcmp(i->first.c_str(), "dll") == 0) {
      dll_name = i->second;
    }

    ++i;
  }


  // open the output DLL
  m_DLL = LoadLibrary(dll_name.c_str());
  if (!m_DLL) {
    return false;
  }

  #define ASSIGN(x) assign(x, GetProcAddress(m_DLL, #x))

  // get the address of all of the functions we need
  ASSIGN(AO_OpenDriver);
  ASSIGN(AO_CloseDriver);
  ASSIGN(AO_Update);
  ASSIGN(AO_OpenStream);
  ASSIGN(AO_CloseStream);
  ASSIGN(AO_PlayStream);
  ASSIGN(AO_StopStream);
  ASSIGN(AO_ResetStream);
  ASSIGN(AO_IsStreamPlaying);
  ASSIGN(AO_SetVolume);
  ASSIGN(AO_GetVolume);

  // if any of the functions are NULL, we failed
  if (!AO_OpenDriver      ||
      !AO_CloseDriver     ||
      !AO_Update          ||
      !AO_OpenStream      ||
      !AO_CloseStream     ||
      !AO_PlayStream      ||
      !AO_StopStream      ||
      !AO_ResetStream     ||
      !AO_IsStreamPlaying ||
      !AO_SetVolume       ||
      !AO_GetVolume) {
    
    FreeLibrary(m_DLL);
    return false;
  }

  // now initialize the driver
  if (!AO_OpenDriver(parameters)) {
    FreeLibrary(m_DLL);
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
DLLOutputContext::Update()
{
  AO_Update();
}

////////////////////////////////////////////////////////////////////////////////

static int ADR_CALL SourceRead(void* opaque, int sample_count, void* samples)
{
  return ((ISampleSource*)opaque)->Read(sample_count, samples);
}

static void ADR_CALL SourceReset(void* opaque)
{
  ((ISampleSource*)opaque)->Reset();
}

IOutputStream*
DLLOutputContext::OpenStream(ISampleSource* source)
{
  int channel_count, sample_rate, bits_per_sample;
  source->GetFormat(channel_count, sample_rate, bits_per_sample);

  AO_STREAM stream = AO_OpenStream(
    channel_count,
    sample_rate,
    bits_per_sample,
    SourceRead,
    SourceReset,
    source);
  if (stream) {
    return new DLLOutputStream(this, stream);
  } else {
    return NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

DLLOutputStream::DLLOutputStream(
  DLLOutputContext* context,
  DLLOutputContext::AO_STREAM stream)
{
  m_Context = context;
  m_Stream  = stream;
}

////////////////////////////////////////////////////////////////////////////////

DLLOutputStream::~DLLOutputStream()
{
  m_Context->AO_CloseStream(m_Stream);
}

////////////////////////////////////////////////////////////////////////////////

void
DLLOutputStream::Play()
{
  m_Context->AO_PlayStream(m_Stream);
}

////////////////////////////////////////////////////////////////////////////////

void
DLLOutputStream::Stop()
{
  m_Context->AO_StopStream(m_Stream);
}

////////////////////////////////////////////////////////////////////////////////

void
DLLOutputStream::Reset()
{
  m_Context->AO_ResetStream(m_Stream);
}

////////////////////////////////////////////////////////////////////////////////

bool
DLLOutputStream::IsPlaying()
{
  return (m_Context->AO_IsStreamPlaying(m_Stream) == AO_TRUE);
}

////////////////////////////////////////////////////////////////////////////////

void
DLLOutputStream::SetVolume(int volume)
{
  m_Context->AO_SetVolume(m_Stream, volume);
}

////////////////////////////////////////////////////////////////////////////////

int
DLLOutputStream::GetVolume()
{
  return m_Context->AO_GetVolume(m_Stream);
}

////////////////////////////////////////////////////////////////////////////////
