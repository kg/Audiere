#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include <stddef.h>

namespace adr {


// data types so we don't require (much of) the standard C++ library
// in VC++, we don't want to link with msvcp60.dll, which few people have


class string
{
public:
  string() { m_length = 0; m_str = NULL; }
  string(const string& rhs);
  ~string() { delete[] m_str; }
  
  string& operator=(const char* str);
  string& operator+=(char c);

  int length() { return m_length; }
  const char* c_str() { return m_str; }

private:
  int   m_length;
  char* m_str;
};


}

#endif
