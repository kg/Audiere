#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <ctype.h>
#include "utility.h"
#include "internal.h"


namespace audiere {

  ParameterList::ParameterList(const char* parameters) {
    std::string key;
    std::string value;

    std::string* current_string = &key;

    // walk the string and generate the parameter list
    const char* p = parameters;
    while (*p) {

      if (*p == '=') {

        current_string = &value;

      } else if (*p == ',') {

        if (key.length() && value.length()) {
          m_values[key] = value;
        }
        key   = "";
        value = "";
        current_string = &key;

      } else {
        *current_string += *p;
      }

      ++p;
    }

    // is there one more parameter without a trailing comma?
    if (key.length() && value.length()) {
      m_values[key] = value;
    }
  }

  std::string
  ParameterList::getValue(std::string key, std::string defaultValue) {
    return (m_values.count(key) ? m_values[key] : defaultValue);
  }


  int strcmp_case(const char* a, const char* b) {
    while (*a && *b) {

      char c = tolower(*a++);
      char d = tolower(*b++);

      if (c != d) {
        return c - d;
      }
    }
  
    char c = tolower(*a);
    char d = tolower(*b);
    return (c - d);
  }


  ADR_EXPORT(int, AdrGetSampleSize)(SampleFormat format) {
    switch (format) {
      case SF_U8:     return 1;
      case SF_S16_LE: return 2;
      default:        return 0;
    }
  }

}
