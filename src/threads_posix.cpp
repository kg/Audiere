#include <unistd.h>
#include <pthread.h>
#include "threads.h"
#include "utility.h"


namespace audiere {

  struct ThreadInternal {
    AI_ThreadRoutine routine;
    void* opaque;
  };


  struct AI_CriticalSectionStruct {
    pthread_mutex_t mutex;
  };


  void* ThreadRoutine(void* arg) {
    ThreadInternal* ti = (ThreadInternal*)arg;
    ti->routine(ti->opaque);
    delete ti;
    return NULL;
  }


  bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority) {
    ThreadInternal* ti = new ThreadInternal;
    ti->routine = routine;
    ti->opaque  = opaque;

    pthread_attr_t attr;
    if (pthread_attr_init(&attr)) {
      delete ti;
      return false;
    }

    // get default scheduling policy
    int policy;
    if (pthread_attr_getschedpolicy(&attr, &policy)) {
      pthread_attr_destroy(&attr);
      delete ti;
      return false;
    }

    int min_prio = sched_get_priority_min(policy);
    int max_prio = sched_get_priority_max(policy);

    // get default scheduling parameters
    sched_param sched;
    if (pthread_attr_getschedparam(&attr, &sched)) {
        pthread_attr_destroy(&attr);
        delete ti;
        return false;
    }

    // treat the specified priority as an offset from the default one
    sched.sched_priority = clamp(
        min_prio,
        sched.sched_priority + priority,
        max_prio);

    if (pthread_attr_setschedparam(&attr, &sched)) {
        pthread_attr_destroy(&attr);
        delete ti;
        return false;
    }

    pthread_t thread;
    int result = pthread_create(&thread, &attr, ThreadRoutine, ti);
    if (result != 0) {
      pthread_attr_destroy(&attr);
      delete ti;
      return false;
    }

    pthread_attr_destroy(&attr);
    return true;
  }


  void AI_Sleep(unsigned milliseconds) {
    int seconds = milliseconds / 1000;
    int useconds = (milliseconds % 1000) * 1000;

    sleep(seconds);
    usleep(useconds);
  }


  AI_CriticalSection AI_CreateCriticalSection() {
    AI_CriticalSectionStruct* cs = new AI_CriticalSectionStruct;
    int result = pthread_mutex_init(&cs->mutex, 0);
    if (result != 0) {
      delete cs;
      return NULL;
    }

    return cs;
  }

  void AI_DestroyCriticalSection(AI_CriticalSection cs) {
    pthread_mutex_destroy(&cs->mutex);
  }

  void AI_EnterCriticalSection(AI_CriticalSection cs) {
    pthread_mutex_lock(&cs->mutex);
  }

  void AI_LeaveCriticalSection(AI_CriticalSection cs) {
    pthread_mutex_unlock(&cs->mutex);
  }

}
