#ifndef PACKET_MANAGER_
#define PACKET_MANAGER_

#include <queue>
#include <functional>
#include <map>
#include "Packet.h"



class PacketManager{
public:
	typedef void (PacketManager::*PROCESS_PACKET_FUNCTION)(const UINT32 clientIndex_, const UINT32 size_, char* pData_);
	typedef std::map<int8_t, PROCESS_PACKET_FUNCTION> MAP_FUNC_LIST; 
    // void Init(){
    //     mPacketFuntionMap[(int)SERVER_ENTER] = &PacketManager::ProcessSystemMessage;
    //     mPacketFuntionMap[(int)SERVER_EXIT] = &PacketManager::ProcessSystemMessage;
    //     mPacketFuntionMap[(int)ROOM_ENTER] = &PacketManager::ProcessSystemMessage;
    //     mPacketFuntionMap[(int)ROOM_EXIT] = &PacketManager::ProcessSystemMessage;
    //     mPacketFuntionMap[(int)CHAT_BROADCAST] = &PacketManager::ProcessBroadCast;
    //     mPacketFuntionMap[(int)CHAT_MULTICAST] = &PacketManager::ProcessMultiCast;
    //     mPacketFuntionMap[(int)CHAT_UNICAST] = &PacketManager::ProcessUniCast;
    // }
    void Run(){

    }

    void ProcessSystemMessage(const UINT32 clientIndex_, const UINT32 size_, char* pData_){

    }

    void ProcessBroadCast(const UINT32 clientIndex_, const UINT32 size_, char* pData_){

    }

    void ProcessBroadCast(const UINT32 clientIndex_, const UINT32 size_, char* pData_){

    }

    void ProcessBroadCast(const UINT32 clientIndex_, const UINT32 size_, char* pData_){

    }



    void ProcessPacket(const UINT32 clientIndex_, const UINT32 size_, char* pData_){
        PACKET_HEADER* packetHeader = (PACKET_HEADER*)pData_;

        switch(packetHeader->Type){
            
        }
    }


private:
    std::map<int, PROCESS_PACKET_FUNCTION> mPacketFuntionMap;

    void EnqueuePacketData(int clientIndex_);
    

};

#endif