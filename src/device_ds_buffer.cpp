#include "device_ds_buffer.h"
#include "device_ds.h"


namespace audiere {

  DSOutputBuffer::DSOutputBuffer(
    DSAudioDevice* device,
    IDirectSoundBuffer* buffer,
    int length,
    int frame_size)
  {
    m_device     = device; // keep the device alive while the buffer exists
    m_buffer     = buffer;
    m_length     = length;
    m_frame_size = frame_size;
    m_repeating  = false;
    m_volume     = 1;
    m_pan        = 0;
  }


  DSOutputBuffer::~DSOutputBuffer() {
    m_buffer->Release();
  }


  void
  DSOutputBuffer::play() {
    m_buffer->Play(0, 0, m_repeating ? DSBPLAY_LOOPING : 0);
  }


  void
  DSOutputBuffer::stop() {
    m_buffer->Stop();
  }


  bool
  DSOutputBuffer::isPlaying() {
    DWORD status;
    HRESULT rv = m_buffer->GetStatus(&status);
    return (SUCCEEDED(rv) && status & DSBSTATUS_PLAYING);
  }


  void
  DSOutputBuffer::reset() {
    setPosition(0);
  }


  void
  DSOutputBuffer::setRepeat(bool repeat) {
    m_repeating = repeat;
    if (isPlaying()) {
      // reset the repeating status by calling Play() on the buffer
      play();
    }
  }


  bool
  DSOutputBuffer::getRepeat() {
    return m_repeating;
  }


  void
  DSOutputBuffer::setVolume(float volume) {
    m_volume = volume;
    m_buffer->SetVolume(DSAudioDevice::Volume_AudiereToDirectSound(volume));
  }


  float
  DSOutputBuffer::getVolume() {
    return m_volume;
  }


  void
  DSOutputBuffer::setPan(float pan) {
    m_pan = pan;
    m_buffer->SetPan(DSAudioDevice::Pan_AudiereToDirectSound(pan));
  }


  float
  DSOutputBuffer::getPan() {
    return m_pan;
  }


  bool
  DSOutputBuffer::isSeekable() {
    return true;
  }


  int
  DSOutputBuffer::getLength() {
    return m_length;
  }


  void
  DSOutputBuffer::setPosition(int position) {
    m_buffer->SetCurrentPosition(position * m_frame_size);
  }


  int
  DSOutputBuffer::getPosition() {
    DWORD play;
    m_buffer->GetCurrentPosition(&play, 0);
    return play / m_frame_size;
  }

}
