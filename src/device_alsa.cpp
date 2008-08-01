#include <algorithm>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>
#include "device_alsa.h"
#include "debug.h"

#define REPORT_ERROR(x) printf ("ALSA error in %s: %s (device: %s)\n", x, snd_strerror (err), device.c_str ())

namespace audiere {

  ALSAAudioDevice*
  ALSAAudioDevice::create(const ParameterList& parameters) {

    std::string device = parameters.getValue("device", "default");

	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;

    // attempt to open output device
    int err = snd_pcm_open (&playback_handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_open");
      return 0;
    }

    // allocate the hardware parameters
    err = snd_pcm_hw_params_malloc (&hw_params);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params_malloc");
      return 0;
    }

    // initialize the hardware parameters
    err = snd_pcm_hw_params_any (playback_handle, hw_params);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params_any");
      return 0;
    }

    // set the access type
    err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params_set_access");
      return 0;
    }

    // set the sample format
    err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params_set_format");
      return 0;
    }

    // set the sample rate
    unsigned int a1 = 44100;
    int a2 = 0;
    err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &a1, &a2);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params_set_rate_near");
      return 0;
    }

    // set the channel count
    err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params_set_channels");
      return 0;
    }

    // set the parameters
    err = snd_pcm_hw_params (playback_handle, hw_params);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_hw_params");
      return 0;
    }

	snd_pcm_hw_params_free (hw_params);

    // prepare
    err = snd_pcm_prepare (playback_handle);
    if (err < 0) {
      REPORT_ERROR ("snd_pcm_prepare");
      return 0;
    }

    return new ALSAAudioDevice(playback_handle);
  }


  ALSAAudioDevice::ALSAAudioDevice(snd_pcm_t *playback_handle)
    : MixerDevice(44100)
  {
    m_playback_handle = playback_handle;
  }


  ALSAAudioDevice::~ALSAAudioDevice() {
    ADR_GUARD("ALSAAudioDevice::~ALSAAudioDevice");
    snd_pcm_close(m_playback_handle);
  }


  void ADR_CALL
  ALSAAudioDevice::update() {
    static const int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE * 4];
    int count = read(BUFFER_SIZE, buffer);
    snd_pcm_writei (m_playback_handle, buffer, count);
  }


  const char* ADR_CALL
  ALSAAudioDevice::getName() {
    return "alsa";
  }

}
