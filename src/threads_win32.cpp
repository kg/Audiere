#include <windows.h>
#include <process.h>
#include "threads.h"


namespace audiere {

  struct ThreadInternal {
    AI_ThreadRoutine routine;
    void*            opaque;
  };

  struct AI_CriticalSectionStruct {
    CRITICAL_SECTION cs;
  };


  static DWORD WINAPI InternalThreadRoutine(void* opaque);


  bool SupportsThreadPriority() {
    // For some reason or another, Windows 95, 98, and ME always
    // deadlock when Audiere tries to use a thread priority other
    // than 0.  Therefore, disable the higher priority on those
    // operating systems.
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(info);
    if (GetVersionEx(&info) &&
        info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      return false;
    } else {
      return true;
    }
  }


  int GetWin32Priority(int priority) {
    if (priority < -2) {
      return THREAD_PRIORITY_IDLE;
    } else if (priority == -2) {
      return THREAD_PRIORITY_LOWEST;
    } else if (priority == -1) {
      return THREAD_PRIORITY_BELOW_NORMAL;
    } else if (priority == 0) {
      return THREAD_PRIORITY_NORMAL;
    } else if (priority == 1) {
      return THREAD_PRIORITY_ABOVE_NORMAL;
    } else {
      return THREAD_PRIORITY_HIGHEST;
    }
  }


  bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority) {
    // create internal thread data
    ThreadInternal* internal = new ThreadInternal;
    internal->routine  = routine;
    internal->opaque   = opaque;
    
    // create the actual thread
    // use _beginthread because it makes a thread-safe C library
    DWORD threadid;
    HANDLE handle = CreateThread(
      0, 0, InternalThreadRoutine, internal,
      CREATE_SUSPENDED, &threadid);
    if (handle) {
      if (SupportsThreadPriority()) {
        SetThreadPriority(handle, GetWin32Priority(priority));
      }
      ResumeThread(handle);
      CloseHandle(handle);
      return true;
    } else {
      return false;
    }
  }


  DWORD WINAPI InternalThreadRoutine(void* opaque) {
    ThreadInternal* internal = (ThreadInternal*)opaque;

    // call the function passed 
    internal->routine(internal->opaque);
    delete internal;
    return 0;
  }


  void AI_Sleep(unsigned milliseconds) {
    ::Sleep(milliseconds);
  }


  AI_CriticalSection AI_CreateCriticalSection() {
    AI_CriticalSectionStruct* cs = new AI_CriticalSectionStruct;
    ::InitializeCriticalSection(&cs->cs);
    return cs;
  }


  void AI_DestroyCriticalSection(AI_CriticalSection cs) {
    ::DeleteCriticalSection(&cs->cs);
    delete cs;
  }


  void AI_EnterCriticalSection(AI_CriticalSection cs) {
    ::EnterCriticalSection(&cs->cs);
  }


  void AI_LeaveCriticalSection(AI_CriticalSection cs) {
    ::LeaveCriticalSection(&cs->cs);
  }

}
