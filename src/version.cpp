#include "audiere.h"
#include "internal.h"


namespace audiere {

  ADR_EXPORT(const char*, AdrGetVersion)() {
    return "1.2.0 " __DATE__ " " __TIME__;
  }

}
