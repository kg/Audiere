#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include <functional>
#include "output_null.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "threads.hpp"
#include "utility.hpp"


////////////////////////////////////////////////////////////////////////////////

NullOutputContext::NullOutputContext()
{
}

////////////////////////////////////////////////////////////////////////////////

NullOutputContext::~NullOutputContext()
{
  ADR_ASSERT(m_streams.size() == 0,
    "Null output context should not die with open streams");
}

////////////////////////////////////////////////////////////////////////////////

bool
NullOutputContext::Initialize(const char* /*parameters*/)
{
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputContext::Update()
{
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
    (*i)->Update();
  }

  AI_Sleep(50);
}

////////////////////////////////////////////////////////////////////////////////

IOutputStream*
NullOutputContext::OpenStream(ISampleSource* source)
{
  NullOutputStream* stream = new NullOutputStream(this, source);
  m_streams.insert(stream);
  return stream;
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputContext::RemoveStream(NullOutputStream* stream) {
  m_streams.erase(stream);
}

////////////////////////////////////////////////////////////////////////////////

NullOutputStream::NullOutputStream(
  NullOutputContext* context,
  ISampleSource* source)
: m_context(context)
, m_source(source)
, m_is_playing(false)
, m_volume(ADR_VOLUME_MAX)
, m_last_update(0)
{
  m_source->GetFormat(m_channel_count, m_sample_rate, m_bits_per_sample);
}

////////////////////////////////////////////////////////////////////////////////

NullOutputStream::~NullOutputStream()
{
  m_context->RemoveStream(this);
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputStream::Play()
{
  m_is_playing = true;
  ResetTimer();
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputStream::Stop()
{
  m_is_playing = false;
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputStream::Reset()
{
  ResetTimer();
}

////////////////////////////////////////////////////////////////////////////////

bool
NullOutputStream::IsPlaying()
{
  return m_is_playing;
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputStream::SetVolume(int volume)
{
  m_volume = volume;
}

////////////////////////////////////////////////////////////////////////////////

int
NullOutputStream::GetVolume()
{
  return m_volume;
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputStream::ResetTimer()
{
  m_last_update = GetNow();
}

////////////////////////////////////////////////////////////////////////////////

void
NullOutputStream::Update()
{
  if (m_is_playing) {
    // get number of milliseconds elapsed since last playing update
    // so we can read that much time worth of samples
    adr_u64 now = GetNow();
    adr_u64 elapsed = now - m_last_update;

    int samples_to_read = m_sample_rate * elapsed / 1000000;
    int samples_read = DummyRead(samples_to_read);

    if (samples_read != samples_to_read) {
      m_source->Reset();
      Stop();
    }

    m_last_update = now;
  }
}

////////////////////////////////////////////////////////////////////////////////

int
NullOutputStream::DummyRead(int samples_to_read)
{
  int total = 0;  // number of samples read so far

  // read samples into dummy buffer, counting the number we actually read
  adr_u8* dummy = new adr_u8[1024 * m_channel_count * m_bits_per_sample / 8];
  while (samples_to_read > 0) {
    int read = adr_max(1024, samples_to_read);
    int actual_read = m_source->Read(read, dummy);
    total += actual_read;
    samples_to_read -= actual_read;
    if (actual_read < read) {
      break;
    }
  }

  delete[] dummy;
  return total;
}

////////////////////////////////////////////////////////////////////////////////
