#pragma comment(lib, "ws2_32")
#include "UserInfo.h"
#include "ClientManager.h"
#include "CriticalSection.h"

#ifndef _CHAT_CLIENT_MANAGER
#define _CHAT_CLIENT_MANAGER

template <typename ClientT>
class ChatClientManager : public ClientManager<ClientT>
{
public:
    ChatClientManager(int maxClientCount): ClientManager<ClientT>(maxClientCount){}

    int FindNickname(char* nickname_){
        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false) continue;
            if(strcmp(nickname_, mClientInfos[i].GetNickname()) == 0) return i;
        }
        return -1;
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
        if(senderRoom == ClientT::LOBBY) return;

        for(int i=0;i<mMaxClientCount;i++){
            if(mClientInfos[i].IsConnected() == false || mClientInfos[i].GetRoom() != senderRoom) continue;
            if(ioType_ == CLOSE && i == senderClientIndex_) {
                mClientInfos[i].SendMsg(size_, pData_, CLOSE);
            }

            mClientInfos[i].SendMsg(size_, pData_);
        }

    }
};
#endif