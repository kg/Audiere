#include "data_types.hpp"
#include <string.h>

namespace adr {


string::string(const string& rhs)
{
  m_length = rhs.m_length;
  m_str = new char[m_length + 1];
  
  if (rhs.m_str) {
    strcpy(m_str, rhs.m_str);
  } else {
    strcpy(m_str, "");
  }
}


string&
string::operator=(const char* str)
{
  delete[] m_str;
  m_length = strlen(str);
  m_str = new char[m_length + 1];
  if (str) {
    strcpy(m_str, str);
  } else {
    strcpy(m_str, "");
  }

  return *this;
}


string&
string::operator+=(char c)
{
  m_length++;

  char* old = m_str;
  m_str = new char[m_length + 1];
  if (old) {
    strcpy(m_str, old);
  }
  delete[] old;
  m_str[m_length - 1] = c;
  m_str[m_length] = 0;

  return *this;
}


}
