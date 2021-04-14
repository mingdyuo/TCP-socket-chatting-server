#include <queue>
#include <mutex>
#ifndef _CLIENT_MANAGER
#define _CLIENT_MANAGER

#define __LOCK_COUNT    mCountMutex.lock();
#define __UNLOCK_COUNT  mCountMutex.unlock();
#define __LOCK_FIND     mFindMutex.lock();
#define __UNLOCK_FIND   mFindMutex.unlock();

template <typename ClientT>
class ClientManager
{

protected:
    int getEmptyClient(){
        __LOCK_FIND
        if (mClientCount == mMaxClientCount) 
        {
            __UNLOCK_FIND
            return mMaxClientCount;
        }

        if(mClientCount == mCurrentCapacity)
        {
            int newCapacity = min(mCurrentCapacity*2, mMaxClientCount);
            mClientInfos.reserve(newCapacity);

            for(int i = mCurrentCapacity;i < newCapacity;i++)
            {
                ClientT* newClient = new ClientT(i);
                mEmptyClients.push(i);
                mClientInfos.push_back(newClient);
            }
            mCurrentCapacity = newCapacity;
        }
        

        int mEmptyIndex = mEmptyClients.front();
        mEmptyClients.pop();

        __UNLOCK_FIND
        return mEmptyIndex;
    }

public:

    ClientManager(int maxClientCount) : mClientCount(0)
    {
        mMaxClientCount = maxClientCount;
        mCurrentCapacity = mMaxClientCount / 4;
        mClientInfos.reserve(mCurrentCapacity);
        for(int i=0;i<mCurrentCapacity;i++)
        {
            ClientT* newClient = new ClientT(i);
            mEmptyClients.push(i);
            mClientInfos.push_back(newClient);
        }
    }
    ~ClientManager()
    {
        for (int i = 0;i < mCurrentCapacity;i++)
        {
            if (mClientInfos[i] != NULL && mClientInfos[i]->IsConnected())
            {
                mClientInfos[i]->Close();
                delete mClientInfos[i];
            }
            else if (mClientInfos[i] != NULL)
            {
                delete mClientInfos[i];
            }
        }
    }

    ClientT* GetClientByIndex(int index) const { return mClientInfos[index]; }
    
    bool CreateClient(HANDLE iocpHandle_, SOCKET socket_)
    {
        int index = getEmptyClient();
        if(index == mMaxClientCount) 
        {
            printf("[알림] Client Full\n");
            return false;
        }

        if (false == mClientInfos[index]->Connect(iocpHandle_, socket_))
        {
            return false;
        }
        

        __LOCK_COUNT
        ++mClientCount;
        __UNLOCK_COUNT

        printf("[알림] Client(%d) 연결 완료 / 현재 연결된 클라이언트 수 : %d / 현재 Capacity : %d\n", index, mClientCount, mClientInfos.size());
        return true;
    }



    void CloseClient(ClientT* client)
    {
        int index = client->GetIndex();
        if(client->IsConnected() == false) return;
        client->Close();

        __LOCK_FIND
        mEmptyClients.push(index);
        __UNLOCK_FIND

        __LOCK_COUNT
        --mClientCount;
        __UNLOCK_COUNT
        printf("[알림] Client(%d) 연결 종료 / 현재 연결된 클라이언트 수 : %d\n", client->GetIndex(), mClientCount);
    }



    void CloseClient(int index_)
    {
        if(mClientInfos[index_]->IsConnected() == false) return;
        mClientInfos[index_]->Close();
        
        __LOCK_FIND
        mEmptyClients.push(index_);
        __UNLOCK_FIND

        __LOCK_COUNT
        --mClientCount;
        __UNLOCK_COUNT
        printf("[알림] Client(%d) 연결 종료 / 현재 연결된 클라이언트 수 : %d\n", index_, mClientCount);
    }




protected:
    std::vector<ClientT*>   mClientInfos;
    std::queue<int>         mEmptyClients;

    int                     mMaxClientCount;
    int                     mCurrentCapacity;
    int                     mClientCount;

    std::mutex              mCountMutex;
    std::mutex              mFindMutex;
    
};


#endif