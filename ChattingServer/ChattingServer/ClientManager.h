#include "CriticalSection.h"
#include <queue>
#ifndef _CLIENT_MANAGER
#define _CLIENT_MANAGER

template <typename ClientT>
class ClientManager
{

protected:
    int getEmptyClient(){
        _LOCK(mFindMutex)
        if (mClientCount == mMaxClientCount) {
            _UNLOCK(mFindMutex)
            return mMaxClientCount;
        }

        if(mClientCount == mCurrentCapacity){
            int newCapacity = min(mCurrentCapacity*2, mMaxClientCount);
            mClientInfos.reserve(newCapacity);
            for(int i = mCurrentCapacity;i < newCapacity;i++){
                ClientT* newClient = new ClientT(i);
                mEmptyClients.push(i);
                mClientInfos.push_back(newClient);
            }
            mCurrentCapacity = newCapacity;
        }
        

        int mEmptyIndex = mEmptyClients.front();
        mEmptyClients.pop();

        _UNLOCK(mFindMutex)
        return mEmptyIndex;
    }

public:

    ClientManager(int maxClientCount):mClientCount(0){
        mMaxClientCount = maxClientCount;
        mCurrentCapacity = mMaxClientCount / 4;
        mClientInfos.reserve(mCurrentCapacity);
        for(int i=0;i<mCurrentCapacity;i++){
            ClientT* newClient = new ClientT(i);
            mEmptyClients.push(i);
            mClientInfos.push_back(newClient);
        }
    }
    ~ClientManager(){}

    ClientT* GetClientByIndex(int index){
        return mClientInfos[index];
    }
    
    bool CreateClient(HANDLE iocpHandle_, SOCKET socket_){
        int index = getEmptyClient();
        if(index == mMaxClientCount) {
            printf("[알림] Client Full\n");
            return false;
        }

        if(false == mClientInfos[index]->Connect(iocpHandle_, socket_)) {
            return false;
        }

        _LOCK(mCountMutex)
        ++mClientCount;
        _UNLOCK(mCountMutex)
        printf("[알림] Client(%d) 연결 완료 / 현재 연결된 클라이언트 수 : %d\n", index, mClientCount);
        return true;
    }



    void CloseClient(ClientT* client){
        if(client->IsConnected() == false) return;
        client->Close();

        _LOCK(mCountMutex)
        --mClientCount;
        _UNLOCK(mCountMutex)
        printf("[알림] Client(%d) 연결 종료 / 현재 연결된 클라이언트 수 : %d\n", client->GetIndex(), mClientCount);
    }



    void CloseClient(int index_){
        if(mClientInfos[index_]->IsConnected() == false) return;
        mClientInfos[index_]->Close();
        
        _LOCK(mCountMutex)
        --mClientCount;
        _UNLOCK(mCountMutex)
        printf("[알림] Client(%d) 연결 종료 / 현재 연결된 클라이언트 수 : %d\n", index_, mClientCount);
    }




protected:
    std::vector<ClientT*>   mClientInfos;
    std::queue<int>         mEmptyClients;

    int                     mMaxClientCount;
    int                     mCurrentCapacity;
    int                     mClientCount;

    CriticalSection         mCountMutex;
    CriticalSection         mFindMutex;
    
};

#endif