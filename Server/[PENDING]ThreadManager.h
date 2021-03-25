#ifndef THREAD_MANAGER_
#define THREAD_MANAGER_

#include <Windows.h>
static const int MAX_THREAD_COUNT = 50;

struct ThreadInfo{
    HANDLE  _handle;
    bool    _isAlive;
};

class ThreadManager{
private:
    ThreadInfo mThreads[MAX_THREAD_COUNT];
public:
    ThreadManager(){}
    ~ThreadManager(){}
template <typename _functionType>
    HANDLE CreateThread(
        _functionType (WINAPI *ThreadFunction)(LPVOID), 
        int ThreadCounts, 
        LPVOID args){
        HANDLE hThread;
        
        hThread = CreateThread(NULL, 0, IOCP::workerThread, args, 0, NULL);
        return hThread;
    }

    void JoinThread(){

    }


private:
    static ThreadManager* mInstance;
public:
    static ThreadManager* GetInstance(){
        if(mInstance == NULL){

        }
    }


};



#endif