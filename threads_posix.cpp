#include <unistd.h>
#include <pthread.h>
#include "threads.hpp"


struct ThreadInternal
{
  AI_ThreadRoutine routine;
  void* opaque;
};


struct CriticalSectionInternal
{
  pthread_mutex_t mutex;
};


////////////////////////////////////////////////////////////////////////////////

void* ThreadRoutine(void* arg)
{
  ThreadInternal* ti = (ThreadInternal*)arg;
  ti->routine(ti->opaque);
  delete ti;
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority)
{
  ThreadInternal* ti = new ThreadInternal;
  ti->routine = routine;
  ti->opaque  = opaque;

  pthread_t thread;
  int result = pthread_create(&thread, NULL, ThreadRoutine, ti);
  if (result != 0) {
    delete ti;
    return false;
  } else {
    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////

void AI_Sleep(unsigned milliseconds)
{
  int seconds = milliseconds / 1000;
  int useconds = (milliseconds % 1000) * 1000;

  sleep(seconds);
  usleep(useconds);
}

////////////////////////////////////////////////////////////////////////////////

AI_CriticalSection AI_CreateCriticalSection()
{
  CriticalSectionInternal* cs = new CriticalSectionInternal;
  int result = pthread_mutex_init(&cs->mutex, NULL);
  if (result != 0) {
    delete cs;
    return NULL;
  }

  return cs;
}

////////////////////////////////////////////////////////////////////////////////

void AI_DestroyCriticalSection(AI_CriticalSection cs)
{
  CriticalSectionInternal* cs_ = (CriticalSectionInternal*)cs;
  pthread_mutex_destroy(&cs_->mutex);
}

////////////////////////////////////////////////////////////////////////////////

void AI_EnterCriticalSection(AI_CriticalSection cs)
{
  CriticalSectionInternal* cs_ = (CriticalSectionInternal*)cs;
  pthread_mutex_lock(&cs_->mutex);
}

////////////////////////////////////////////////////////////////////////////////

void AI_LeaveCriticalSection(AI_CriticalSection cs)
{
  CriticalSectionInternal* cs_ = (CriticalSectionInternal*)cs;
  pthread_mutex_unlock(&cs_->mutex);
}

////////////////////////////////////////////////////////////////////////////////
