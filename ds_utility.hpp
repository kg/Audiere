#ifndef DS_UTILITY_HPP
#define DS_UTILITY_HPP


#include <dsound.h>
#include <math.h>


static const int DS_DefaultBufferLength = 1000;  // one second


// DirectSound treats volumes as decibels (exponential growth like the Richter
// scale).  We want a linear ramp.  Do the conversion!


inline int Volume_AudiereToDirectSound(int volume) {
  // I can't figure out the proper math, and this comes close enough...
  double fv = volume / 255.0;  // range: 0-1
  double attenuate = pow(1 - fv, 3);
  return (-10000 * attenuate);
}


#endif
