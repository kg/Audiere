/*
 * Internal platform-specific threads
 *
 * AI namespace = Audiere Internal
 *
 */


#ifndef THREADS_HPP
#define THREADS_HPP


// threads
typedef void (*AI_ThreadRoutine)(void* opaque);
extern bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority = 0);

// waiting
extern void AI_Sleep(unsigned milliseconds);

// critical section
typedef void* AI_CriticalSection;
extern AI_CriticalSection AI_CreateCriticalSection();
extern void AI_DestroyCriticalSection(AI_CriticalSection cs);
extern void AI_EnterCriticalSection(AI_CriticalSection cs);
extern void AI_LeaveCriticalSection(AI_CriticalSection cs);


#endif
