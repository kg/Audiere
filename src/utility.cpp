#include <ctype.h>
#include "utility.hpp"


////////////////////////////////////////////////////////////////////////////////

int strcmp_case(const char* a, const char* b)
{
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

////////////////////////////////////////////////////////////////////////////////
