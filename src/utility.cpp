#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <ctype.h>
#include "utility.h"


namespace audiere {

  ParameterList ParseParameters(const char* parameter_string) {
    ParameterList theList;

    std::string key;
    std::string value;

    std::string* current_string = &key;

    // walk the string and generate the parameter list
    const char* p = parameter_string;
    while (*p) {

      if (*p == '=') {

        current_string = &value;

      } else if (*p == ',') {

        if (key.length() && value.length()) {
          theList.push_back(Parameter(key, value));
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
      theList.push_back(Parameter(key, value));
    }

    return theList;
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

}
