#ifndef MCI_DEVICE_H
#define MCI_DEVICE_H


#include <windows.h>
#include <mmsystem.h>
#include "debug.h"


namespace audiere {

  /**
   * Provides a nice interface to an MCI device name.  Closes the
   * device on destruction.
   */
  class MCIDevice {
  public:
    MCIDevice(const std::string& device) {
      m_device = device;
    }

    ~MCIDevice() {
      sendCommand("close");
    }

  protected:
    std::string sendCommand(const std::string& request, const std::string& parameters = "") {
      return sendString(request + " " + m_device + " " + parameters);
    }

    static std::string sendString(const std::string& string, bool* error = 0) {
      ADR_LOG("Sending MCI Command: " + string);

      const int bufferLength = 1000;
      char buffer[bufferLength + 1] = {0};
      MCIERROR e = mciSendString(string.c_str(), buffer, 1000, NULL);
      if (error) {
        *error = (e != 0);
      }

      char errorString[bufferLength + 1] = {0};
      mciGetErrorString(e, errorString, bufferLength);

      if (e) {
        ADR_LOG("Error: " + std::string(errorString));
      }
      if (buffer[0]) {
        ADR_LOG("Result: " + std::string(buffer));
      }
      return buffer;
    }

  private:
    std::string m_device;
  };

}


#endif
