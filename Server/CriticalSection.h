
#include <windows.h>

#ifndef _CRITICAL_SECTION
#define _CRITICAL_SECTION


class CriticalSection
{
    CRITICAL_SECTION mCritSec;
public:
    CriticalSection(){
        InitializeCriticalSection(&mCritSec);
    }

    ~CriticalSection() {
        DeleteCriticalSection(&mCritSec);
    }

    void Lock() 
    {
        EnterCriticalSection(&mCritSec);
    }
    
    void Unlock() 
    {
        LeaveCriticalSection(&mCritSec);
    }

};

#endif