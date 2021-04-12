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
        return -1; // 주석처리 하지 않을 시 닉네임 중복 허용 (스트레스 서버 테스트)

        for(int i=0;i<ClientManager<ClientT>::mCurrentCapacity;i++){
            if(ClientManager<ClientT>::mClientInfos[i]->IsConnected() == false) continue;
            if(strcmp(nickname_, ClientManager<ClientT>::mClientInfos[i]->GetNickname()) == 0) return i;
        }
        return -1;
        
    }

    void SetNickname(const PacketInfo& packetInfo_){
        SERVER_ENTER_PACKET* packet = (SERVER_ENTER_PACKET*)packetInfo_.pPacketData;
        ClientManager<ClientT>::mClientInfos[packetInfo_.ClientIndex]->SetNickname(packet->Sender);
        printf("[알림] Client(%d) 닉네임 생성 완료 [%s]\n", packetInfo_.ClientIndex, packet->Sender);
    }

    void UniCast(const PacketInfo& packetInfo_){
        ClientManager<ClientT>::mClientInfos[packetInfo_.ClientIndex]->SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData);
    }

    void BroadCast(const PacketInfo& packetInfo_){
        for(int i=0;i<ClientManager<ClientT>::mCurrentCapacity;i++){
            if(ClientManager<ClientT>::mClientInfos[i]->IsConnected() == false ) continue;
            ClientManager<ClientT>::mClientInfos[i]->SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData);
        }
    }

    void MultiCast(const PacketInfo& packetInfo_, IOOperation ioType_ = SEND){
        int senderIndex = packetInfo_.ClientIndex;
        int senderRoom = ClientManager<ClientT>::mClientInfos[senderIndex]->GetRoom();
        if(senderRoom == ClientT::LOBBY && ClientManager<ClientT>::mClientInfos[senderIndex]->GetPrevRoom() == ClientT::LOBBY) return;
        if(senderRoom == ClientT::LOBBY) senderRoom = ClientManager<ClientT>::mClientInfos[senderIndex]->GetPrevRoom();

        for(int i=0;i<ClientManager<ClientT>::mCurrentCapacity;i++){
            if(ClientManager<ClientT>::mClientInfos[i]->IsConnected() == false || ClientManager<ClientT>::mClientInfos[i]->GetRoom() != senderRoom) continue;
            if(ioType_ == CLOSE && i == senderIndex) {
                ClientManager<ClientT>::mClientInfos[i]->SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData, CLOSE);
                continue;
            }

            ClientManager<ClientT>::mClientInfos[i]->SendMsg(packetInfo_.DataSize, packetInfo_.pPacketData);
        }

    }
};
#endif