#include <audiere.h>
#include "mozAudioService.h"
#include "mozAudioContext.h"


mozAudioService::mozAudioService()
{
  NS_INIT_REFCNT();
}


NS_IMPL_ISUPPORTS1(mozAudioService, mozIAudioService);


NS_IMETHODIMP
mozAudioService::CreateContext(mozIAudioContext** rv)
{
  if (!rv) {
    return NS_ERROR_NULL_POINTER;
  }

  ADR_CONTEXT context = AdrCreateContext(0);
  if (context) {

    *rv = new mozAudioContext(context);
    NS_ADDREF(*rv);
    return NS_OK;

  } else {

    *rv = NULL;
    return NS_ERROR_FAILURE;

  }
}
