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
        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false) return i;
        }
        return mMaxClientCount;
    }

public:

    ClientManager<ClientT>(int maxClientCount){
        mMaxClientCount = maxClientCount;
        for(int i=0;i<mMaxClientCount;i++){
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
        ++mUserCount;
        _UNLOCK(mMutex)
        printf("[알림] Client(%d) 연결 완료\n", index);
        return true;
    }

    void CloseClient(ClientT* client){
        client->Close();

        _LOCK(mMutex)
        --mUserCount;
        _UNLOCK(mMutex)
    }



    void CloseClient(int index_){
        mClientInfos[index_].Close();
        
        _LOCK(mMutex)
        --mUserCount;
        _UNLOCK(mMutex)
    }




protected:
    std::vector<ClientT>   mClientInfos;

    int mMaxClientCount;
    int mUserCount;

    CriticalSection mMutex;
    
};

#endif