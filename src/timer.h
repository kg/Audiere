/**
 * @file
 *
 * Platform-independent abstraction for high-resolution timers.
 */


#ifndef TIMER_HPP
#define TIMER_HPP


#include "types.h"


namespace audiere {

  /// Return current time in microseconds.
  u64 GetNow();

}


#endif
