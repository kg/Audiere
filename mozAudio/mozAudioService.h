#ifndef MOZ_AUDIO_SERVICE_H
#define MOZ_AUDIO_SERVICE_H


#include "mozIAudioService.h"


// 8b9b900c-1dd2-11b2-a044-f0047f4a00b3
#define MOZ_AUDIO_SERVICE_CID \
  { 0x8b9b900c, 0x1dd2, 0x11b2, { 0xa0, 0x44, 0xf0, 0x04, 0x7f, 0x4a, 0x00, 0xb3 } }

#define MOZ_AUDIO_SERVICE_CONTRACTID "@aegisknight.org/audio;1"


class mozAudioService : public mozIAudioService
{
public:
  mozAudioService();

  NS_DECL_ISUPPORTS
  NS_DECL_MOZIAUDIOSERVICE
};


#endif
