#include <nsMemory.h>
#include "mozAudioStream.h"


mozAudioStream::mozAudioStream(ADR_STREAM stream)
{
  NS_INIT_REFCNT();

  mStream = stream;
}


mozAudioStream::~mozAudioStream()
{
  AdrCloseStream(mStream);
}


NS_IMPL_ISUPPORTS1(mozAudioStream, mozIAudioStream);


NS_IMETHODIMP
mozAudioStream::Play()
{
  AdrPlayStream(mStream);
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::Pause()
{
  AdrPauseStream(mStream);
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::Reset()
{
  AdrResetStream(mStream);
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::GetRepeating(PRBool* aRepeating)
{
  if (!aRepeating) {
    return NS_ERROR_NULL_POINTER;
  }

  *aRepeating = (AdrGetStreamRepeat(mStream) ? PR_TRUE : PR_FALSE);
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::SetRepeating(PRBool aRepeating)
{
  AdrSetStreamRepeat(mStream, (aRepeating ? ADR_TRUE : ADR_FALSE));
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::GetVolume(PRInt16* aVolume)
{
  if (!aVolume) {
    return NS_ERROR_NULL_POINTER;
  }

  *aVolume = (PRInt16)AdrGetStreamVolume(mStream);
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::SetVolume(PRInt16 aVolume)
{
  AdrSetStreamVolume(mStream, aVolume);
  return NS_OK;
}


NS_IMETHODIMP
mozAudioStream::GetPlaying(PRBool* aPlaying)
{
  *aPlaying = (AdrIsStreamPlaying(mStream) ? PR_TRUE : PR_FALSE);
  return NS_OK;
}
