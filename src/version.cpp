#include "audiere.h"
#include "internal.h"


namespace audiere {

  ADR_EXPORT(const char*) AdrGetVersion() {
    return "Audiere 1.9.3 - " __DATE__ " - " __TIME__;
  }

}
