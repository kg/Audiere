#include "audiere.h"

#include <fstream>
#include <string>
#include <vector>

#ifdef CDAUDIO_WINDOWS
#    include <windows.h>
#    include <mmsystem.h>
#    include <stdio.h>

    static void doMci(const std::string& request, const std::string& parameters, std::string& returned) {
        std::string command = request+" "+deviceName+" "+parameters;
        char buffer[1000];
        mciSendString(command.c_str(), buffer, 1000, NULL);
        returned = buffer;
    }
    static void doMci(const std::string& request, const std::string& parameters) {
        std::string dummy;
        doMci(request, parameters, dummy);
    }
#else
#    include <cdaudio.h>
#endif

namespace audiere {

  CDDrive::CDDrive(const std::string& device)
  {
      deviceName = device;
#ifdef CDAUDIO_WINDOWS
      std::string command = "open "+device+":\\ type cdaudio alias cd"+device;
      char buffer[1000];
      mciSendString(command.c_str(), buffer, 1000, NULL);
#else
      desc = cd_init_device(const_cast<char*>(deviceName.c_str()));
      if( desc < 0 )
      {
          perror("Could not open CD drive");
      }
#endif
  }
  
  CDDrive::~CDDrive()
  {
#ifdef CDAUDIO_WINDOWS
      doMci("close", "");
#else
      if( desc > 0 )
      {
          cd_close(desc);
      }
#endif
  }
  
  void 
  CDDrive::stop()
  {
#ifdef CDAUDIO_WINDOWS
      doMci("stop", "");
#else
      if( desc>=0 ) cd_stop(desc);
#endif
  }
  
  void 
  CDDrive::play(long track)
  {
#ifdef CDAUDIO_WINDOWS
      char buffer[1000];
      sprintf(buffer, "from %ld to %ld", track, track);
      doMci("play", buffer);
#else
      if( desc>=0 ) cd_play_track(desc, track+1, track+1);
#endif
  }
  
  void 
  CDDrive::pause()
  {
#ifdef CDAUDIO_WINDOWS
      doMci("pause", "");
#else
      if( desc>=0 ) cd_stop(desc);
      if( desc>=0 ) cd_pause(desc);
#endif
  }
  
  void 
  CDDrive::resume()
  {
#ifdef CDAUDIO_WINDOWS
      doMci("resume", "");
#else
      if( desc>=0 ) cd_resume(desc);
#endif
  }
  
  bool 
  CDDrive::isPlaying()
  {
#ifdef CDAUDIO_WINDOWS
      std::string status;
      doMci("status", "mode", status);
      return (status=="playing");
#else
      if( desc < 0 ) return false;
      disc_status status;
      cd_poll(desc, &status);
      return (status.status_mode == CDAUDIO_PLAYING);
#endif
  }

  bool
  CDDrive::containsCD()
  {
#ifdef CDAUDIO_WINDOWS
      std::string status;
      doMci("status", "media present", status);
      return (status=="TRUE");
#else
      if( desc<0 ) return false;

      struct disc_status status;
      cd_poll(desc, &status);

      return status.status_present;
#endif
  }
}

