#include <string>
#include "audiere.h"
#include "internal.h"
#include "mci_device.h"


namespace audiere {

  /**
   * Since MCI uses a global list of device aliases, this prevents name
   * collisions. Maybe should be synchronized or volatile.
   */
  static int g_song_count = 0;


  class MCIMIDIStream : public RefImplementation<MIDIStream>, public MCIDevice {
  public:
    static MCIMIDIStream* create(const char* filename) {
      // Synchronize on g_song_count?
      char alias[80];
      sprintf(alias, "adr_song_%d", g_song_count);

      bool error;
      std::string result = sendString(
        std::string("open \"") + filename + "\" type sequencer alias " + alias,
        &error);
      if (error) {
        return 0;
      }
      
      ++g_song_count;
      return new MCIMIDIStream(alias);
    }

    MCIMIDIStream(const std::string& device)
      : MCIDevice(device)
    {
      sendCommand("set", "time format milliseconds");
    }

    ADR_METHOD(void) play() {
      if (isPlaying()) {
        return;
      }

      if (getPosition() == getLength()) {
        setPosition(0);
      }
      sendCommand("play");
    }

    ADR_METHOD(void) stop() {
      pause();
      setPosition(0);
    }

    ADR_METHOD(void) pause() {
      sendCommand("stop");
    }

    ADR_METHOD(bool) isPlaying() {
      return sendCommand("status", "mode") == "playing";
    }

    ADR_METHOD(int) getLength() {
      return atoi(sendCommand("status", "length").c_str());
    }

    ADR_METHOD(int) getPosition() {
      int pos = atoi(sendCommand("status", "position").c_str());
      int length = getLength();
      return (length == 0 ? pos : pos % length);
    }

    ADR_METHOD(void) setPosition(int position) {
      bool playing = isPlaying();

      char buffer[80];
      sprintf(buffer, "%d", position);
      sendCommand("seek", std::string("to ") + buffer);

      if (playing) {
        play();
      }
    }

  private:
    std::string m_device;
  };


  class MCIMIDIDevice : public RefImplementation<MIDIDevice> {
  public:
    ADR_METHOD(const char*) getName() {
      return "MCI";
    }

    ADR_METHOD(MIDIStream*) openStream(const char* filename) {
      return MCIMIDIStream::create(filename);
    }
  };


  namespace hidden {

    ADR_EXPORT(MIDIDevice*) AdrOpenMIDIDevice(const char* /*name*/) {
      return new MCIMIDIDevice;
    }

  }

}
