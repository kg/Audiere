#include "OutputStream.h"


OutputStream::OutputStream(audiere::OutputStream* stream) {
  NS_INIT_ISUPPORTS();
  m_stream = stream;
}


NS_IMPL_ISUPPORTS1(OutputStream, adrIOutputStream);


NS_IMETHODIMP
OutputStream::GetPlaying(PRBool* aPlaying) {
  *aPlaying = (m_stream->isPlaying() ? PR_TRUE : PR_FALSE);
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::SetPlaying(PRBool aPlaying) {
  if (aPlaying) {
    m_stream->play();
  } else {
    m_stream->stop();
  }
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::GetRepeating(PRBool* aRepeating) {
  *aRepeating = (m_stream->getRepeat() ? PR_TRUE : PR_FALSE);
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::SetRepeating(PRBool aRepeating) {
  m_stream->setRepeat(aRepeating == PR_TRUE);
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::GetVolume(float* volume) {
  *volume = m_stream->getVolume();
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::SetVolume(float volume) {
  m_stream->setVolume(volume);
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::GetPan(float* pan) {
  *pan = m_stream->getPan();
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::SetPan(float pan) {
  m_stream->setPan(pan);
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::GetSeekable(PRBool* aSeekable) {
  *aSeekable = (m_stream->isSeekable() ? PR_TRUE : PR_FALSE);
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::GetLength(PRInt32* length) {
  *length = m_stream->getLength();
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::GetPosition(PRInt32* position) {
  *position = m_stream->getPosition();
  return NS_OK;
}


NS_IMETHODIMP
OutputStream::SetPosition(PRInt32 position) {
  m_stream->setPosition(position);
  return NS_OK;
}
