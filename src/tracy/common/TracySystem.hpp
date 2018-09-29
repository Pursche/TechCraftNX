#ifndef __TRACYSYSTEM_HPP__
#define __TRACYSYSTEM_HPP__

#ifdef TRACY_ENABLE
#  if defined __ANDROID__ || defined __CYGWIN__ || defined __APPLE__ || defined _GNU_SOURCE || ( defined _WIN32 && ( !defined NTDDI_WIN10_RS2 || NTDDI_VERSION < NTDDI_WIN10_RS2 ) )
#    define TRACY_COLLECT_THREAD_NAMES
#  endif
#endif

#ifdef __SWITCH__
    #include <threads.h>
#else
#ifdef _WIN32
#ifndef _WINDOWS_
extern "C" __declspec(dllimport) unsigned long __stdcall GetCurrentThreadId(void);
#endif
#else
#  include <pthread.h>
#endif
#endif

#include <stdint.h>
#include <thread>

namespace tracy
{

static inline uint64_t GetThreadHandle()
{
#ifdef __SWITCH__
    return (uint64_t)thrd_current();
#elif defined _WIN32
    static_assert( sizeof( decltype( GetCurrentThreadId() ) ) <= sizeof( uint64_t ), "Thread handle too big to fit in protocol" );
    return uint64_t( GetCurrentThreadId() );
#elif defined __APPLE__
    uint64_t id;
    pthread_threadid_np( pthread_self(), &id );
    return id;
#else
    static_assert( sizeof( decltype( pthread_self() ) ) <= sizeof( uint64_t ), "Thread handle too big to fit in protocol" );
    return uint64_t( pthread_self() );
#endif
}

//void SetThreadName( thrd_t& thread, const char* name );
#ifdef __SWITCH__
inline void SetThreadName(thrd_t handle, const char* name ){};
inline const char* GetThreadName(uint64_t id){ return "UNSUPPORTED"; };
#else
void SetThreadName( thrd_t handle, const char* name );
const char* GetThreadName( uint64_t id );
#endif

}

#endif
