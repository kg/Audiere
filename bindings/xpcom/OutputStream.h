#ifndef MOZ_AUDIO_STREAM_H
#define MOZ_AUDIO_STREAM_H


#include <audiere.h>
#include "adrIOutputStream.h"


class OutputStream : public adrIOutputStream {
public:
  OutputStream(audiere::OutputStream* stream);

  NS_DECL_ISUPPORTS
  NS_DECL_ADRIOUTPUTSTREAM

private:
  audiere::RefPtr<audiere::OutputStream> m_stream;
};


#endif
