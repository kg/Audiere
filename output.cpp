#include <string.h>
#include "output.hpp"
#include "output_null.hpp"

#ifdef _WIN32
#include "output_ds8.hpp"
#include "output_ds3.hpp"
#include "output_dll.hpp"
#endif


////////////////////////////////////////////////////////////////////////////////

IOutputContext* OpenContext(const char* device, const char* parameters)
{
  IOutputContext* context;

  #define TRY_CONTEXT(context_type)            \
    context = new context_type();              \
    if (context->Initialize(parameters)) {     \
      return context;                          \
    } else {                                   \
      delete context;                          \
    }

  // autodetect
  if (strcmp(device, "autodetect") == 0) {

    TRY_CONTEXT(DS8OutputContext)
    TRY_CONTEXT(DS3OutputContext)
    TRY_CONTEXT(NullOutputContext)

  // DirectSound
  } else if (strcmp(device, "directsound") == 0) {

    TRY_CONTEXT(DS8OutputContext)
    TRY_CONTEXT(DS3OutputContext)

  // null
  } else if (strcmp(device, "null") == 0) {

    TRY_CONTEXT(NullOutputContext);

  // dll
  } else if (strcmp(device, "dll") == 0) {

    TRY_CONTEXT(DLLOutputContext);

  }

  // otherwise, just fail

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void ParseParameters(
  const char* parameter_string,
  ParameterList& parameters)
{
  adr::string key;
  adr::string value;

  adr::string* current_string = &key;

  // walk the string and generate the parameter list
  const char* p = parameter_string;
  while (*p) {

    if (*p == '=') {

      current_string = &value;

    } else if (*p == ',') {

      if (key.length() && value.length()) {
        parameters.push_back(Parameter(key, value));
      }
      key   = "";
      value = "";
      current_string = &key;

    } else {
      *current_string += *p;
    }

    p++;
  }

  // is there one more parameter without a trailing comma?
  if (key.length() && value.length()) {
    parameters.push_back(Parameter(key, value));
  }
}

////////////////////////////////////////////////////////////////////////////////
