#pragma comment(lib, "ws2_32")
#include "UserInfo.h"
#include "ClientManager.h"
#include "CriticalSection.h"
#include "PacketInfo.h"

#ifndef _CHAT_CLIENT_MANAGER
#define _CHAT_CLIENT_MANAGER

template <typename ClientT>
class ChatClientManager : public ClientManager<ClientT>
{
public:
    ChatClientManager(int maxClientCount): ClientManager<ClientT>(maxClientCount){}

    int FindNickname(char* nickname_){
        for(int i=0;i<mCurrentCapacity;i++){
            if(mClientInfos[i].IsConnected() == false) continue;
            if(strcmp(nickname_, mClientInfos[i].GetNickname()) == 0) return i;
        }
        return -1;
    }

    void SetNickname(const PacketInfo& packetInfo_){
        SERVER_ENTER_PACKET* packet = (SERVER_ENTER_PACKET*)packetInfo_.pPacketData;
        mClientInfos[packetInfo_.ClientIndex].SetNickname(packet->Sender);
        
    }

    char* MoveData(int index_, int ioSize_){
        mClientInfos[index_].SetSendBuf(ioSize_);
        return mClientInfos[index_].SendBuffer();
    }

    void UniCast(const PacketInfo& packetInfo_){
        mClientInfos[packetInfo_.ClientIndex].SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData);
    }

    void BroadCast(const PacketInfo& packetInfo_){
        for(int i=0;i<mCurrentCapacity;i++){
            if(mClientInfos[i].IsConnected() == false ) continue;
            mClientInfos[i].SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData);
        }
    }

    void MultiCast(const PacketInfo& packetInfo_, IOOperation ioType_ = SEND){
        int senderIndex = packetInfo_.ClientIndex;
        int senderRoom = mClientInfos[senderIndex].GetRoom();
        if(senderRoom == ClientT::LOBBY && mClientInfos[senderIndex].GetPrevRoom() == ClientT::LOBBY) return; 
        if(senderRoom == ClientT::LOBBY) senderRoom = mClientInfos[senderIndex].GetPrevRoom();

        for(int i=0;i<mCurrentCapacity;i++){
            if(mClientInfos[i].IsConnected() == false || mClientInfos[i].GetRoom() != senderRoom) continue;
            if(ioType_ == CLOSE && i == senderIndex) {
                mClientInfos[i].SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData, CLOSE);
            }

            mClientInfos[i].SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData);
        }

    }
};
#endif