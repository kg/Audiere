#include "audiere.h"
#include "internal.h"


namespace audiere {

  ADR_EXPORT(const char*, AdrGetVersion)() {
    return "Audiere 1.9.2 - " __DATE__ " - " __TIME__;
  }

}
