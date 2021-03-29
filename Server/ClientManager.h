#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>
#include "ClientInfo.h"

#ifndef _CLIENT_MANAGER
#define _CLIENT_MANAGER

class ClientManager
{

private:
    int getEmptyClient(){
        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false) return i;
        }
        return mMaxClientCount;
    }

public:

    ClientManager(int maxClientCount){
        mMaxClientCount = maxClientCount;
        for(int i=0;i<mMaxClientCount;i++){
            mClientInfos.push_back(stClientInfo(i));
        }
    }
    ~ClientManager(){}

    stClientInfo* GetClientByIndex(int index){
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

        ++mUserCount;
        printf("[알림] Client(%d) 연결 완료\n", index);
    }

    void CloseClient(stClientInfo* client){
        client->Close();
        --mUserCount;
    }

    int FindNickname(char* nickname_){
        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false) continue;
            if(strcmp(nickname_, mClientInfos[i].GetNickname()) == 0) return i;
        }
        return -1;
    }


    void CloseClient(int index_){
        mClientInfos[index_].Close();
        --mUserCount;
    }

    char* MoveData(int index_, int ioSize_){
        mClientInfos[index_].SetSendBuf(ioSize_);
        return mClientInfos[index_].SendBuffer();
    }

    void BroadCast(const UINT32 senderClientIndex_, const UINT32 size_, char* pData_){
        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false ) continue;
            mClientInfos[i].SendMsg(size_, pData_);
        }
    }

    void MultiCast(const UINT32 senderClientIndex_, const UINT32 size_, char* pData_, IOOperation ioType_ = SEND){
        int senderRoom = mClientInfos[senderClientIndex_].GetRoom();
        if(senderRoom == stClientInfo::LOBBY) return;

        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false || mClientInfos[i].GetRoom() != senderRoom) continue;
            if(ioType_ == CLOSE && i == senderClientIndex_) {
                mClientInfos[i].SendMsg(size_, pData_, CLOSE);
            }

            mClientInfos[i].SendMsg(size_, pData_);
        }

    }


private:
    std::vector<stClientInfo>   mClientInfos;

    int mMaxClientCount;
    int mUserCount;
    
};

#endif