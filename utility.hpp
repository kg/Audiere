#ifndef UTILITY_HPP
#define UTILITY_HPP


int strcmp_case(const char* a, const char* b);


// we can't use std::min or std::max in VC++ 6, so define our own

template<typename T>
inline T adr_min(T a, T b) {
  return (a < b ? a : b);
}

template<typename T>
inline T adr_max(T a, T b) {
  return (a > b ? a : b);
}


#endif
