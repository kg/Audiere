#ifndef ACQ_INTERNAL_HPP
#define ACQ_INTERNAL_HPP


#include "acoustique.h"


// should match the C runtime's calling convention
#define CRT_CALL __cdecl


// these are platform-specific...  put them somewhere else
typedef unsigned char  acq_u8;
typedef signed   char  acq_s8;
typedef unsigned short acq_u16;
typedef signed   short acq_s16;
typedef unsigned long  acq_u32;
typedef signed   long  acq_s32;


struct ACQ_STREAMimp
{
  void*              opaque;
  ACQ_READ_CALLBACK  read;
  ACQ_RESET_CALLBACK reset;
  int                stream_format;

  // set by stream_open
  int num_channels;
  int bits_per_sample;
  int sample_rate;

  // stream_format-specific
  bool (*stream_open) (ACQ_STREAM stream);
  void (*stream_close)(ACQ_STREAM stream);
  int  (*stream_read) (ACQ_STREAM stream,
                       void* samples,
                       int sample_count);
  bool (*stream_reset)(ACQ_STREAM stream);
  
  // used for stream_format private data
  void* internal;
};


// we can't use std::min or std::max in VC++ 6, so define our own

template<typename T>
inline T acq_min(T a, T b) {
  return (a < b ? a : b);
}

template<typename T>
inline T acq_max(T a, T b) {
  return (a > b ? a : b);
}


#define DECLARE_STATE(name)                                \
  extern bool name##_Open (ACQ_STREAM stream);   \
  extern void name##_Close(ACQ_STREAM stream);   \
  extern int  name##_Read (ACQ_STREAM stream,    \
                           void* samples,                  \
                           int sample_count);              \
  extern bool name##_Reset(ACQ_STREAM stream);

DECLARE_STATE(MOD)  // acq_mod.cpp
DECLARE_STATE(MP3)  // acq_mp3.cpp
DECLARE_STATE(OGG)  // acq_ogg.cpp
DECLARE_STATE(WAV)  // acq_wav.cpp


#endif
