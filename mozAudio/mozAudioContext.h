#ifndef MOZ_AUDIO_CONTEXT_H
#define MOZ_AUDIO_CONTEXT_H


#include <audiere.h>
#include "mozIAudioContext.h"


class mozAudioContext : public mozIAudioContext
{
public:
  mozAudioContext(ADR_CONTEXT context);
  ~mozAudioContext();

  NS_DECL_ISUPPORTS
  NS_DECL_MOZIAUDIOCONTEXT

private:
  ADR_CONTEXT mContext;
};


#endif
