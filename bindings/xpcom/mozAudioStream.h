#ifndef MOZ_AUDIO_STREAM_H
#define MOZ_AUDIO_STREAM_H


#include <audiere.h>
#include "mozIAudioStream.h"
#include "mozIAudioContext.h"


class mozAudioStream : public mozIAudioStream
{
public:
  mozAudioStream(ADR_STREAM stream);
  ~mozAudioStream();

  NS_DECL_ISUPPORTS
  NS_DECL_MOZIAUDIOSTREAM

private:
  ADR_STREAM mStream;
};


#endif
