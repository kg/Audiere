#include <windows.h>
#include <process.h>
#include "threads.hpp"


struct ThreadInternal
{
  AI_ThreadRoutine routine;
  void*         opaque;
  int           priority;
};

struct AI_CriticalSectionStruct
{
  CRITICAL_SECTION cs;
};


static void __cdecl InternalThreadRoutine(void* opaque);


////////////////////////////////////////////////////////////////////////////////

bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority)
{
  // create internal thread data
  ThreadInternal* internal = new ThreadInternal;
  internal->routine  = routine;
  internal->opaque   = opaque;
  internal->priority = priority;

  // create the actual thread
  // use _beginthread because it makes a thread-safe C library
  unsigned long handle = ::_beginthread(InternalThreadRoutine, 0, internal);
  return (handle != -1);
}

////////////////////////////////////////////////////////////////////////////////

void __cdecl InternalThreadRoutine(void* opaque)
{
  ThreadInternal* internal = (ThreadInternal*)opaque;

  int priority;
  if (internal->priority < -2) {
    priority = THREAD_PRIORITY_IDLE;
  } else if (internal->priority == -2) {
    priority = THREAD_PRIORITY_LOWEST;
  } else if (internal->priority == -1) {
    priority = THREAD_PRIORITY_BELOW_NORMAL;
  } else if (internal->priority == 0) {
    priority = THREAD_PRIORITY_NORMAL;
  } else if (internal->priority == 1) {
    priority = THREAD_PRIORITY_ABOVE_NORMAL;
  } else {
    priority = THREAD_PRIORITY_HIGHEST;
  }

  // set the priority
  SetThreadPriority(GetCurrentThread(), priority);

  // call the function passed 
  internal->routine(internal->opaque);
  delete internal;
}

////////////////////////////////////////////////////////////////////////////////

void AI_Sleep(unsigned milliseconds)
{
  ::Sleep(milliseconds);
}

////////////////////////////////////////////////////////////////////////////////

AI_CriticalSection AI_CreateCriticalSection()
{
  AI_CriticalSectionStruct* cs = new AI_CriticalSectionStruct;
  ::InitializeCriticalSection(&cs->cs);
  return cs;
}

////////////////////////////////////////////////////////////////////////////////

void AI_DestroyCriticalSection(AI_CriticalSection cs)
{
  ::DeleteCriticalSection(&cs->cs);
  delete cs;
}

////////////////////////////////////////////////////////////////////////////////

void AI_EnterCriticalSection(AI_CriticalSection cs)
{
  ::EnterCriticalSection(&cs->cs);
}

////////////////////////////////////////////////////////////////////////////////

void AI_LeaveCriticalSection(AI_CriticalSection cs)
{
  ::LeaveCriticalSection(&cs->cs);
}

////////////////////////////////////////////////////////////////////////////////
