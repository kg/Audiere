#include <nsMemory.h>
#include "mozAudioContext.h"
#include "mozAudioStream.h"


mozAudioContext::mozAudioContext(ADR_CONTEXT context)
{
  NS_INIT_REFCNT();
  mContext = context;
}


mozAudioContext::~mozAudioContext()
{
  AdrDestroyContext(mContext);
}


NS_IMPL_ISUPPORTS1(mozAudioContext, mozIAudioContext);


NS_IMETHODIMP
mozAudioContext::OpenStream(const char* path, mozIAudioStream** rv)
{
  // sanity checks
  if (!path || !rv) {
    return NS_ERROR_NULL_POINTER;
  }

  // open the audio stream
  ADR_STREAM stream = AdrOpenStream(mContext, path);
  if (!stream) {
    *rv = NULL;
    return NS_ERROR_FAILURE;
  }

  *rv = new mozAudioStream(stream);
  (*rv)->AddRef();
  return NS_OK;
}
