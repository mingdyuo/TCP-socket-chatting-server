//#include "PacketManager.h"
//
///*
//    1. Packet Thread Run
//    2. Packet queue - Enqueue
//    3. Packet queue - Dequeue
//*/
//
//template <typename ClientT>
//void PacketManager<ClientT>::Run()
//{
//    while(mbIsPacketRun)
//    {
//        PacketInfo packetData = DequeuePacket();
//
//        if(packetData.DataSize == 0)
//        {
//            Sleep(60);
//        }
//        else
//        {   
//            switch(packetData.SendType){
//                case SENDTYPE_BROAD: 
//                    mClientMgr->BroadCast(packetData);
//                    break;
//                case SENDTYPE_MULTI:
//                    mClientMgr->MultiCast(packetData, packetData.isClose ? CLOSE : SEND);
//                    break;
//                case SENDTYPE_UNI:
//                    mClientMgr->UniCast(packetData);
//                    break;
//                default:    
//                    break;  
//            }
//            packetData.Release();
//        }
//    }
//}
//
//
//template <typename ClientT>
//void PacketManager<ClientT>::EnqueuePacket(PacketInfo& packetInfo_)
//{
//    __LOCKQUEUE
//    mPacketQueue.push(packetInfo_);
//    __UNLOCKQUEUE
//}
//
//
//template <typename ClientT>
//PacketInfo PacketManager<ClientT>::DequeuePacket()
//{
//    __LOCKQUEUE
//    if(mPacketQueue.empty())
//    {
//        __UNLOCKQUEUE
//        return PacketInfo();
//    }
//    PacketInfo packet = mPacketQueue.front();
//    mPacketQueue.pop();
//    __UNLOCKQUEUE
//
//    return packet;
//}
//
///*
//    Packet process functions mapped by eAction value
//*/
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_SERVER_ENTER(PacketInfo& packetInfo_)
//{
//    SERVER_ENTER_PACKET* recvPacket = (SERVER_ENTER_PACKET*)packetInfo_.pPacketData;
//    int existIndex = mClientMgr->FindNickname(recvPacket->Sender);
//
//    if (existIndex < 0)     //< No duplicate nickname
//    {
//        mClientMgr->SetNickname(packetInfo_);
//    }
//
//
//    packetInfo_.pPacketData = new char[SERVER_MESSAGE_PACKET_LENGTH];
//    SERVER_MESSAGE_PACKET* sendPacket = (SERVER_MESSAGE_PACKET*)packetInfo_.pPacketData;
//    sendPacket->Type = SERVER_MESSAGE;
//    sendPacket->Length = SERVER_MESSAGE_PACKET_LENGTH;
//    sendPacket->Message = existIndex < 0 ? NICKNAME_CREATED : NICKNAME_ALREADY_EXIST;
//
//    packetInfo_.DataSize = SERVER_MESSAGE_PACKET_LENGTH;
//    packetInfo_.SendType    = SENDTYPE_UNI;
//
//    EnqueuePacket(packetInfo_);
//
//}
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_CHAT_UNICAST(PacketInfo& packetInfo_)
//{
//    UNICAST_PACKET* packet = (UNICAST_PACKET*)packetInfo_.pPacketData;
//
//    packetInfo_.SendType = SENDTYPE_UNI;
//    
//    int findIndex = mClientMgr->FindNickname(packet->Recver);
//    
//    if(findIndex > -1)      //< If target user exist
//    {
//        PacketInfo copied = PacketInfo(packetInfo_);
//        EnqueuePacket(copied);
//        copied.ClientIndex = findIndex;
//        EnqueuePacket(copied);
//    }
//    else                   //< Target user doesn't exist
//    {
//        SERVER_MESSAGE_PACKET* sendPacket = new SERVER_MESSAGE_PACKET();
//        sendPacket->Type = SERVER_MESSAGE;
//        sendPacket->Length = SERVER_MESSAGE_PACKET_LENGTH;
//        sendPacket->Message = NICKNAME_NOT_FOUND;
//
//        packetInfo_.DataSize = sendPacket->Length;
//        packetInfo_.pPacketData = (char*) sendPacket;
//
//        EnqueuePacket(packetInfo_);
//    }
//}
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_SERVER_EXIT(PacketInfo& packetInfo_)
//{
//    PacketInfo copied = PacketInfo(packetInfo_);
//    copied.SendType = SENDTYPE_MULTI;
//    copied.isClose = true;
//    EnqueuePacket(copied);
//}
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_ROOM_ENTER(PacketInfo& packetInfo_)
//{
//    ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)packetInfo_.pPacketData;
//    ClientT* client_ = mClientMgr->GetClientByIndex(packetInfo_.ClientIndex);
//    client_->EnterRoom(packet->RoomNo);
//
//    PacketInfo copied = PacketInfo(packetInfo_);
//    copied.SendType = SENDTYPE_MULTI;
//    EnqueuePacket(copied);
//}
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_ROOM_EXIT(PacketInfo& packetInfo_)
//{  
//    ClientT* client_ = mClientMgr->GetClientByIndex(packetInfo_.ClientIndex);
//    client_->ExitRoom(); 
//
//    PacketInfo copied = PacketInfo(packetInfo_);
//    copied.SendType = SENDTYPE_MULTI;
//    EnqueuePacket(copied);
//}
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_CHAT_BROADCAST(PacketInfo& packetInfo_)
//{
//    PacketInfo copied = PacketInfo(packetInfo_);
//    copied.SendType = SENDTYPE_BROAD;
//    EnqueuePacket(copied);
//}
//
//template <typename ClientT>
//void PacketManager<ClientT>::PROCESS_CHAT_MULTICAST(PacketInfo& packetInfo_)
//{
//    PacketInfo copied = PacketInfo(packetInfo_);
//    copied.SendType = SENDTYPE_MULTI;
//    EnqueuePacket(copied);
//}
