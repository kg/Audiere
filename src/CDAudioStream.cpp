#include "audiere.h"

namespace audiere {
  CDAudioStream::CDAudioStream(CDDrive* device, int track)
  {
      theDrive = device;
      trackNum = track;
  }
  
  void ADR_CALL
  CDAudioStream::play()
  {
      theDrive->play(trackNum);
  }

  void ADR_CALL
  CDAudioStream::stop()
  {
      theDrive->stop();
  }

  void ADR_CALL
  CDAudioStream::pause()
  {
      theDrive->pause();
  }

  void ADR_CALL
  CDAudioStream::resume()
  {
      theDrive->resume();
  }

  bool ADR_CALL
  CDAudioStream::isPlaying()
  {
      return theDrive->isPlaying();
  }

  void ADR_CALL
  CDAudioStream::reset()
  {
      // not supported
  }

  void ADR_CALL
  CDAudioStream::setRepeat(bool repeat)
  {
      // not supported
  }

  bool ADR_CALL
  CDAudioStream::getRepeat()
  {
      return false;
  }

  void ADR_CALL
  CDAudioStream::setVolume(float volume)
  {
      // not supported
  }

  float ADR_CALL
  CDAudioStream::getVolume()
  {
      return 1;
  }

  void ADR_CALL
  CDAudioStream::setPan(float pan)
  {
      // not supported
  }

  float ADR_CALL
  CDAudioStream::getPan()
  {
      return 0;
  }

  void ADR_CALL
  CDAudioStream::setPitchShift(float pan)
  {
      // not supported
  }

  float ADR_CALL
  CDAudioStream::getPitchShift()
  {
      return 1;
  }

  bool ADR_CALL
  CDAudioStream::isSeekable()
  {
      return false;
      // TODO: can we make it seekable???
  }

  int ADR_CALL
  CDAudioStream::getLength()
  {
      return 0;
  }

  void ADR_CALL
  CDAudioStream::setPosition(int position)
  {
      // not supported
  }

  int ADR_CALL
  CDAudioStream::getPosition()
  {
      return 0;
  }

  CDAudioStream* OpenCDSound(CDDrive* theDrive, int track) {return new CDAudioStream(theDrive, track);}

}

