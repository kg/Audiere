#include <unistd.h>
#include <fcntl.h>
#include <ioctl.h>
#include <sys/soundcard.h>
#include "output_oss.hpp"


////////////////////////////////////////////////////////////////////////////////

OSSOutputContext::OSSOutputContext()
{
  m_output_file = -1;
}

////////////////////////////////////////////////////////////////////////////////

OSSOutputContext::~OSSOutputContext()
{
  if (m_output_file != -1) {
    close(m_output_file);
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
OSSOutputContext::Initialize(const char* parameters)
{
  ParameterList pl;
  ParseParameters(parameters, pl);

  std::string device = "/dev/dsp";  // default device

  ParameterList::iterator i;
  for (i = pl.begin(); i != pl.end(); ++i) {
    if (i->first == "device") {
      device = i->second;
    }
  }

  // attempt to open output device
  m_output_device = open(device.c_str(), O_WRONLY);
  if (m_output_device == -1) {
    perror(device.c_str());
    return false;
  }

  int format = AFMT_S16_LE;
  if (ioctl(m_output_device, SNDCTL_DSP_SETFMT, &format) == -1) {
    perror("SNDCTL_DSP_SETFMT");
    return false;
  }
  if (format != AFMT_S16_LE) {
    // unsupported format
    return false;
  }

  int stereo = 1;
  if (ioctl(m_output_device, SNDCTL_DSP_STEREO, &stereo) == -1) {
    perror("SNDCTL_DSP_STEREO");
    return false;
  }
  if (stereo != 1) {
    // unsupported channel number
    return false;
  }

  int speed = 44100;
  if (ioctl(m_output_device, SNDCTL_DSP_SPEED, &speed) == -1) {
    perror("SNDCTL_DSP_SPEED");
    return false;
  }
  if (abs(44100 - speed) > 2205) {
    // unsupported sampling rate
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
OSSOutputContext::Update()
{
  char buffer[1024 * 4];
  m_mixer->Read(buffer, 1024);
  // write to the OSS device
}

////////////////////////////////////////////////////////////////////////////////

IOutputStream* OpenStream(ISampleSource source)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void
OSSOutputStream::Play()
{
}

////////////////////////////////////////////////////////////////////////////////

void
OSSOutputStream::Stop()
{
}

////////////////////////////////////////////////////////////////////////////////

void
OSSOutputStream::Reset()
{
}

////////////////////////////////////////////////////////////////////////////////

bool
OSSOutputStream::IsPlaying()
{
}

////////////////////////////////////////////////////////////////////////////////

void
OSSOutputStream::SetVolume(int volume)
{
}

////////////////////////////////////////////////////////////////////////////////

int
OSSOutputStream::GetVolume()
{
}

////////////////////////////////////////////////////////////////////////////////
