/* MPEG Sound library

   (C) 1997 by Woo-jae Jung */

// Soundinputstream.cc
// Abstractclass of inputstreams

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "mpegsound.h"

Soundinputstream::Soundinputstream()
{
  __errorcode=SOUND_ERROR_OK;
};

Soundinputstream::~Soundinputstream()
{
  // Nothing...
};
