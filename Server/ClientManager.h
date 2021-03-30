#pragma comment(lib, "ws2_32")
// #include "UserInfo.h"
#include "CriticalSection.h"

#ifndef _CLIENT_MANAGER
#define _CLIENT_MANAGER

template <typename ClientT>
class ClientManager
{

protected:
    int getEmptyClient(){
        if(mClientCount == mMaxClientCount)
            return mMaxClientCount;
        
        if(mClientCount == mCurrentCapacity){
            int newCapacity = min(mCurrentCapacity*2, mMaxClientCount);
            mClientInfos.reserve(newCapacity);
            for(int i = mCurrentCapacity;i < newCapacity;i++){
                mClientInfos.push_back(ClientT(i));
            }
            mCurrentCapacity = newCapacity;
        }
        for(int i=0;i<mCurrentCapacity;i++){
            if(mClientInfos[i].IsConnected() == false) return i;
        }
        return mMaxClientCount;
    }

public:

    ClientManager<ClientT>(int maxClientCount){
        mMaxClientCount = maxClientCount;
        mCurrentCapacity = mMaxClientCount / 4;
        mClientInfos.reserve(mCurrentCapacity);
        for(int i=0;i<mCurrentCapacity;i++){
            mClientInfos.push_back(ClientT(i));
        }
    }
    ~ClientManager(){}

    ClientT* GetClientByIndex(int index){
        return &mClientInfos[index];
    }
    
    bool CreateClient(HANDLE iocpHandle_, SOCKET socket_){
        int index = getEmptyClient();
        if(index == mMaxClientCount) {
            printf("[에러] Client Full\n");
            return false;
        }

        bool bConnected = mClientInfos[index].Connect(iocpHandle_, socket_);
        if(bConnected == false) {
            return false;
        }
        _LOCK(mMutex)
        ++mClientCount;
        _UNLOCK(mMutex)
        printf("[알림] Client(%d) 연결 완료\n", index);
        return true;
    }

    void CloseClient(ClientT* client){
        client->Close();

        _LOCK(mMutex)
        --mClientCount;
        _UNLOCK(mMutex)
    }



    void CloseClient(int index_){
        mClientInfos[index_].Close();
        
        _LOCK(mMutex)
        --mClientCount;
        _UNLOCK(mMutex)
    }




protected:
    std::vector<ClientT>   mClientInfos;

    int mMaxClientCount;
    int mCurrentCapacity;
    int mClientCount;

    CriticalSection mMutex;
    
};

#endif