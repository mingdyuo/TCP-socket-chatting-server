#include "PacketManager.h"

template <typename ClientT>
void PacketManager<ClientT>::Run(){
    while(mbIsPacketRun){
        PacketInfo packetData = DequeuePacket();

        if(packetData.DataSize == 0)
        {
            Sleep(60);
        }
        else
        {   
            switch(packetData.SendType){
                case SENDTYPE_BROAD: 
                    mClientMgr->BroadCast(packetData);
                    break;
                case SENDTYPE_MULTI:
                    mClientMgr->MultiCast(packetData, packetData.isClose ? CLOSE : SEND);
                    break;
                case SENDTYPE_UNI:
                    mClientMgr->UniCast(packetData);
                    break;
                default:    
                    break;  
            }
            packetData.Release();
        }
    }
}


template <typename ClientT>
void PacketManager<ClientT>::EnqueuePacket(PacketInfo& packetInfo_){
    _LOCK(mCs)
    mPacketQueue.push(packetInfo_);
    _UNLOCK(mCs)
}


template <typename ClientT>
PacketInfo PacketManager<ClientT>::DequeuePacket(){
    _LOCK(mCs)
    if(mPacketQueue.empty()){
        _UNLOCK(mCs)
        return PacketInfo();
    }
    PacketInfo packet = mPacketQueue.front();
    mPacketQueue.pop();
    _UNLOCK(mCs)

    return packet;
}


template <typename ClientT>
void PacketManager<ClientT>::PROCESS_SERVER_ENTER(PacketInfo& packetInfo_){
    SERVER_ENTER_PACKET* recvPacket = (SERVER_ENTER_PACKET*)packetInfo_.pPacketData;
    int existIndex = mClientMgr->FindNickname(recvPacket->Sender);

    if(existIndex < 0 )
        mClientMgr->SetNickname(packetInfo_);

    packetInfo_.pPacketData = new char[SERVER_MESSAGE_PACKET_LENGTH];
    SERVER_MESSAGE_PACKET* sendPacket = (SERVER_MESSAGE_PACKET*)packetInfo_.pPacketData;
    sendPacket->Type = SERVER_MESSAGE;
    sendPacket->Length = SERVER_MESSAGE_PACKET_LENGTH;
    sendPacket->Message = existIndex < 0 ? NICKNAME_CREATED : NICKNAME_ALREADY_EXIST;

    packetInfo_.DataSize = SERVER_MESSAGE_PACKET_LENGTH;
    packetInfo_.SendType    = SENDTYPE_UNI;

    EnqueuePacket(packetInfo_);

}

template <typename ClientT>
void PacketManager<ClientT>::PROCESS_CHAT_UNICAST(PacketInfo& packetInfo_)
{
    UNICAST_PACKET* packet = (UNICAST_PACKET*)packetInfo_.pPacketData;

    packetInfo_.SendType = SENDTYPE_UNI;
    
    int findIndex = mClientMgr->FindNickname(packet->Recver);
    
    if(findIndex > -1){     //< If target user exist
        EnqueuePacket(packetInfo_);
        packetInfo_.ClientIndex = findIndex;
        EnqueuePacket(packetInfo_);
    }
    else{                   //< Target user doesn't exist
        SERVER_MESSAGE_PACKET* sendPacket = new SERVER_MESSAGE_PACKET();
        sendPacket->Type = SERVER_MESSAGE;
        sendPacket->Length = SERVER_MESSAGE_PACKET_LENGTH;
        sendPacket->Message = NICKNAME_NOT_FOUND;

        packetInfo_.DataSize = sendPacket->Length;
        packetInfo_.pPacketData = (char*) sendPacket;

        EnqueuePacket(packetInfo_);
    }
}

template <typename ClientT>
void PacketManager<ClientT>::PROCESS_SERVER_EXIT(PacketInfo& packetInfo_)
{
    packetInfo_.SendType = SENDTYPE_MULTI;
    packetInfo_.isClose = true;
    EnqueuePacket(packetInfo_);
}

template <typename ClientT>
void PacketManager<ClientT>::PROCESS_ROOM_ENTER(PacketInfo& packetInfo_)
{

    ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)packetInfo_.pPacketData;
    ClientT* client_ = mClientMgr->GetClientByIndex(packetInfo_.ClientIndex);
    client_->EnterRoom(packet->RoomNo);

    packetInfo_.SendType = SENDTYPE_MULTI;
    EnqueuePacket(packetInfo_);
}

template <typename ClientT>
void PacketManager<ClientT>::PROCESS_ROOM_EXIT(PacketInfo& packetInfo_)
{  
    ClientT* client_ = mClientMgr->GetClientByIndex(packetInfo_.ClientIndex);
    client_->ExitRoom(); 

    packetInfo_.SendType = SENDTYPE_MULTI;
    EnqueuePacket(packetInfo_);
}

template <typename ClientT>
void PacketManager<ClientT>::PROCESS_CHAT_BROADCAST(PacketInfo& packetInfo_)
{
    BROADCAST_PACKET* packet = (BROADCAST_PACKET*)packetInfo_.pPacketData;
    packetInfo_.SendType = SENDTYPE_BROAD;
    EnqueuePacket(packetInfo_);
}

template <typename ClientT>
void PacketManager<ClientT>::PROCESS_CHAT_MULTICAST(PacketInfo& packetInfo_)
{
    packetInfo_.SendType = SENDTYPE_MULTI;
    EnqueuePacket(packetInfo_);
}
