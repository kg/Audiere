#ifndef DEVICE_H
#define DEVICE_H


#include "audiere.h"


namespace audiere {
  
  /// Basic StopEvent implementation.
  class StopEventImpl : public RefImplementation<StopEvent> {
  public:
    StopEventImpl(OutputStream* os, Reason reason) {
      m_stream = os;
      m_reason = reason;
    }

    OutputStream* ADR_CALL getOutputStream() {
      return m_stream.get();
    }

    Reason ADR_CALL getReason() {
      return m_reason;
    }

  private:
    OutputStreamPtr m_stream;
    Reason m_reason;
  };


  /// Contains default implementation of functionality common to all devices.
  class AbstractDevice : public RefImplementation<AudioDevice> {
  public:
    void ADR_CALL registerStopCallback(StopCallback* callback) {
      m_callbacks.push_back(callback);
    }

    void ADR_CALL unregisterStopCallback(StopCallback* callback) {
      for (size_t i = 0; i < m_callbacks.size(); ++i) {
        if (m_callbacks[i] == callback) {
          m_callbacks.erase(m_callbacks.begin() + i);
          return;
        }
      }
    }

    void ADR_CALL clearStopCallbacks() {
      m_callbacks.clear();
    }

  protected:
    void fireStopEvent(OutputStream* stream, StopEvent::Reason reason) {
      StopEventPtr event = new StopEventImpl(stream, reason);
      fireStopEvent(event);
    }

    void fireStopEvent(const StopEventPtr& event) {
      fireStopEvent(event.get());
    }

    void fireStopEvent(StopEvent* event) {
      for (size_t i = 0; i < m_callbacks.size(); ++i) {
        m_callbacks[i]->streamStopped(event);
      }
    }

  private:
    std::vector<StopCallbackPtr> m_callbacks;
  };

}


#endif
