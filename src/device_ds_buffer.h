#include <audiere.h>


namespace audiere {

  class DSOutputBuffer : public RefImplementation<SampleSource> {
  private:
/*
    DSOutputBuffer(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      void* samples, int sample_count,
*/
  public:
    void play();
    void stop();
    bool isPlaying();
    void reset();

    void setRepeat(bool repeat);
    bool getRepeat();

    void  setVolume(float volume);
    float getVolume();

    void  setPan(float pan);
    float getPan();

    bool isSeekable();
    int  getLength();
    void setPosition(int position);
    int  getPosition();

  private:
    
  };

}
