#include <windows.h>
#include <mmsystem.h>
#include "timer.hpp"


static adr_u64 s_frequency;


adr_u64 GetNow() {
  // do we have a known frequency yet?
  if (s_frequency == 0) {
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency)) {
      s_frequency = frequency.QuadPart;
    }
  }

  // if we have a frequency, try to use the performance counter
  if (s_frequency != 0) {
    LARGE_INTEGER now;
    if (QueryPerformanceCounter(&now)) {
      return 1000000 * adr_u64(now.QuadPart) / s_frequency;
    }
  }

  // no performance counter, so use the Win32 multimedia timer
  return timeGetTime() * 1000;
}
