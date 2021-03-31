
#include <queue>
#include <functional>
#include <map>
#include "Packet.h"
#include "PacketInfo.h"
#include "CriticalSection.h"
#include <queue>

#ifndef PACKET_MANAGER_
#define PACKET_MANAGER_


template <typename ClientT>
class PacketManager{
public:
    bool mbIsPacketRun;

    
    PacketManager(ChatClientManager<ClientT>* clientMgr_):mbIsPacketRun(true)
    {
        // 함수 매핑하기
        mClientMgr = clientMgr_;
        
        mPacketFunctionMap[SERVER_ENTER]    = &PacketManager::PROCESS_SERVER_ENTER;
        mPacketFunctionMap[SERVER_EXIT]     = &PacketManager::PROCESS_SERVER_EXIT;

        mPacketFunctionMap[ROOM_ENTER]      = &PacketManager::PROCESS_ROOM_ENTER;
        mPacketFunctionMap[ROOM_EXIT]       = &PacketManager::PROCESS_ROOM_EXIT;

        mPacketFunctionMap[CHAT_BROADCAST]  = &PacketManager::PROCESS_CHAT_BROADCAST;
        mPacketFunctionMap[CHAT_MULTICAST]  = &PacketManager::PROCESS_CHAT_MULTICAST;
        mPacketFunctionMap[CHAT_UNICAST]    = &PacketManager::PROCESS_CHAT_UNICAST;

    }

	typedef void (PacketManager::*PROCESS_PACKET_FUNCTION)(PacketInfo& packetInfo);
	typedef std::map<UINT16, PROCESS_PACKET_FUNCTION> MAP_FUNC_LIST; 

    bool PacketThread(){
        mPacketThread = (HANDLE)_beginthreadex(NULL, 0, StaticPacketThread, this, 0, NULL);
        if(mPacketThread == NULL){
            printf("[에러] packetThread() 함수 실패\n");
            return false;
        }
        return true;
    }


    static unsigned __stdcall StaticPacketThread(void* arg){
        PacketManager<ClientT>* This = (PacketManager<ClientT>*)arg;
        This->Run();
        return 0;
    };

    void Run(){
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


    void Close(){
        mbIsPacketRun = false;
        WaitForSingleObject(mPacketThread, INFINITE);
        CloseHandle(mPacketThread);
        printf("[알림] PacketThread 종료\n");
    }
    

    PacketInfo DequeuePacket(){
        if(mPacketQueue.empty()){
            return PacketInfo();
        }
        _LOCK(mCs)
        PacketInfo packet = mPacketQueue.front();
        mPacketQueue.pop();
        _UNLOCK(mCs)

        return packet;
    }


    void ClassifyPacket(PacketInfo& packetInfo_){
        PACKET_HEADER* packetHeader = (PACKET_HEADER*)packetInfo_.pPacketData;

        //< 패킷 동적 할당은 mapping된 함수로 이동한 후 실행
        PROCESS_PACKET_FUNCTION fp = mPacketFunctionMap[packetHeader->Type];
        if(fp != NULL) (this->*fp)(packetInfo_);
    }



private:


    void EnqueuePacket(PacketInfo& packetInfo_){
        _LOCK(mCs)
        mPacketQueue.push(packetInfo_);
        _UNLOCK(mCs)
    }

    void PROCESS_SERVER_ENTER(PacketInfo& packetInfo_){
        SERVER_ENTER_PACKET* recvPacket = (SERVER_ENTER_PACKET*)packetInfo_.pPacketData;
        int existIndex = mClientMgr->FindNickname(recvPacket->Sender);

        packetInfo_.pPacketData = new char[SERVER_MESSAGE_PACKET_LENGTH];
        SERVER_MESSAGE_PACKET* sendPacket = (SERVER_MESSAGE_PACKET*)packetInfo_.pPacketData;
        sendPacket->Type = SERVER_MESSAGE;
        sendPacket->Length = SERVER_MESSAGE_PACKET_LENGTH;

        if(existIndex >=0 ){
            sendPacket->Message = NICKNAME_ALREADY_EXIST;
        }
        else{
            sendPacket->Message = NICKNAME_CREATED;
            mClientMgr->SetNickname(packetInfo_);
            //< TODO : Create 하는 부분을 옮겨서 실제 닉네임 세팅해야함 (체크 필요)
        }

        packetInfo_.DataSize = SERVER_MESSAGE_PACKET_LENGTH;
        packetInfo_.SendType    = SENDTYPE_UNI;

        EnqueuePacket(packetInfo_);

    }

    void PROCESS_CHAT_UNICAST(PacketInfo& packetInfo_)
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

    void PROCESS_SERVER_EXIT(PacketInfo& packetInfo_)
    {
        packetInfo_.SendType = SENDTYPE_MULTI;
        packetInfo_.isClose = true;
        // mClientMgr->CloseClient(packetInfo_.ClientIndex); //< TODO : 이거 순서 바꿔야 하는디;;
        EnqueuePacket(packetInfo_);
    }

    void PROCESS_ROOM_ENTER(PacketInfo& packetInfo_)
    {

        ROOM_ENTER_PACKET* packet = (ROOM_ENTER_PACKET*)packetInfo_.pPacketData;
        ClientT* client_ = mClientMgr->GetClientByIndex(packetInfo_.ClientIndex);
        client_->EnterRoom(packet->RoomNo);

        packetInfo_.SendType = SENDTYPE_MULTI;
        EnqueuePacket(packetInfo_);
    }

    void PROCESS_ROOM_EXIT(PacketInfo& packetInfo_)
    {  
        ClientT* client_ = mClientMgr->GetClientByIndex(packetInfo_.ClientIndex);
        client_->ExitRoom(); 

        packetInfo_.SendType = SENDTYPE_MULTI;
        EnqueuePacket(packetInfo_);
    }

    void PROCESS_CHAT_BROADCAST(PacketInfo& packetInfo_)
    {
        packetInfo_.SendType = SENDTYPE_BROAD;
        EnqueuePacket(packetInfo_);
    }

    void PROCESS_CHAT_MULTICAST(PacketInfo& packetInfo_)
    {
        packetInfo_.SendType = SENDTYPE_MULTI;
        EnqueuePacket(packetInfo_);
    }


    ChatClientManager<ClientT>* mClientMgr;
    std::queue<PacketInfo>      mPacketQueue;

    MAP_FUNC_LIST               mPacketFunctionMap;

    CriticalSection             mCs;

    HANDLE                      mPacketThread;

};

#endif