#ifdef WIN32
  #include <windows.h>
#endif

#include "debug.h"


namespace audiere {

  FILE* Log::handle;
  int Log::indent_count;


  void
  Log::Write(const char* str)
  {
    std::string s(std::string(indent_count * 2, ' ') + str + "\n");

    #ifdef WIN32
      OutputDebugString(s.c_str());
    #endif

    EnsureOpen();
    if (handle) {
      fputs(s.c_str(), handle);
      fflush(handle);
    }
  }


  void
  Log::EnsureOpen()
  {
    if (!handle) {

      #ifdef WIN32
        handle = fopen("C:/audiere_debug.log", "w");
      #else
        std::string home(getenv("HOME"));
        handle = fopen((home + "/audiere_debug.log").c_str(), "w");
      #endif

      atexit(Close);
    }
  }


  void
  Log::Close()
  {
    fclose(handle);
  }

}
