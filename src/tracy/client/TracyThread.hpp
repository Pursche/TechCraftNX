#ifndef __TRACYTHREAD_HPP__
#define __TRACYTHREAD_HPP__

#ifdef _MSC_VER
#  include <windows.h>
#elif defined __SWITCH_
#include <threads.h>
#else
#  include <pthread.h>
#endif

namespace tracy
{

#ifdef _MSC_VER

class Thread
{
public:
    Thread( void(*func)( void* ptr ), void* ptr )
        : m_func( func )
        , m_ptr( ptr )
        , m_hnd( CreateThread( nullptr, 0, Launch, this, 0, nullptr ) )
    {}

    ~Thread()
    {
        WaitForSingleObject( m_hnd, INFINITE );
        CloseHandle( m_hnd );
    }

    HANDLE Handle() const { return m_hnd; }

private:
    static DWORD WINAPI Launch( void* ptr ) { ((Thread*)ptr)->m_func( ((Thread*)ptr)->m_ptr ); return 0; }

    void(*m_func)( void* ptr );
    void* m_ptr;
    HANDLE m_hnd;
};
#elif defined __SWITCH__

class Thread
{
public:
    Thread( void(*func)( void* ptr ), void* ptr )
        : m_func( func )
        , m_ptr( ptr )
    {
        thrd_create( &m_thread, Launch, this );
    }

    ~Thread()
    {
        thrd_join( m_thread, nullptr );
    }

    thrd_t Handle() const { return m_thread; }

private:
    static int Launch( void* ptr ) { ((Thread*)ptr)->m_func( ((Thread*)ptr)->m_ptr ); return 0; }
    void(*m_func)( void* ptr );
    void* m_ptr;
    thrd_t m_thread;
};

#else

class Thread
{
public:
    Thread( void(*func)( void* ptr ), void* ptr )
        : m_func( func )
        , m_ptr( ptr )
    {
        pthread_create( &m_thread, nullptr, Launch, this );
    }

    ~Thread()
    {
        pthread_join( m_thread, nullptr );
    }

    pthread_t Handle() const { return m_thread; }

private:
    static void* Launch( void* ptr ) { ((Thread*)ptr)->m_func( ((Thread*)ptr)->m_ptr ); return nullptr; }
    void(*m_func)( void* ptr );
    void* m_ptr;
    pthread_t m_thread;
};

#endif

}

#endif
