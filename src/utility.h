#ifndef UTILITY_HPP
#define UTILITY_HPP


#include <list>
#include <string>
#include <utility>
#include "audiere.h"


namespace audiere {

  // we can't use std::min or std::max in VC++ 6, so define our own

  template<typename T>
  inline T Min(T a, T b) {
    return (a < b ? a : b);
  }

  template<typename T>
  inline T Max(T a, T b) {
    return (a > b ? a : b);
  }

  typedef std::pair<std::string, std::string> Parameter;
  typedef std::list<Parameter> ParameterList;

  ParameterList ParseParameters(const char* parameters_string);

  int strcmp_case(const char* a, const char* b);


  class UnseekableSource : public DLLImplementation<SampleSource> {
  public:
    bool isSeekable() {
      return false;
    }

    int getLength() {
      return 0;
    }

    void setPosition(int /*position*/) {
    }

    int getPosition() {
      return 0;
    }
  };

}


#endif
