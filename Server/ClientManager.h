#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>
#include "ClientInfo.h"
#define MAX_SOCKBUF 1024	//패킷 크기


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
    }

    void CloseClient(stClientInfo* client){
        client->Close();
    }

    void CloseClient(int index_){
        mClientInfos[index_].Close();
    }

    char* MoveData(int index_, int ioSize_){
        mClientInfos[index_].SetSendBuf(ioSize_);
        return mClientInfos[index_].GetSendBuf();
    }

    void SetNickname(stClientInfo* client, char* nickname_){
        client->SetNickname(nickname_);
    }

    void BroadCast(const UINT32 senderClientIndex_, const UINT32 size_, char* pData_){
        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false || i == senderClientIndex_) continue;
            mClientInfos[i].SendMsg(size_, pData_);
        }
    }

    void MultiCast(const UINT32 senderClientIndex_, const UINT32 size_, char* pData_){
        int senderRoom = mClientInfos[senderClientIndex_].GetRoom();
        if(senderRoom == stClientInfo::LOBBY) return;

        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false || i == senderClientIndex_ || mClientInfos[i].GetRoom() != senderRoom) continue;

            mClientInfos[i].SendMsg(size_, pData_);
        }
    }


private:
    std::vector<stClientInfo> mClientInfos;

    int mMaxClientCount;
    int mUserCount;
    
};