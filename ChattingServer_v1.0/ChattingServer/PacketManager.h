#pragma once
#include <queue>
#include <functional>
#include <map>
#include "Packet.h"
#include "PacketInfo.h"
#include <queue>
#include <thread>
#include <mutex>
using std::thread;


#ifndef PACKET_MANAGER_
#define PACKET_MANAGER_


template <typename ClientT>
class PacketManager
{
public:
    
    PacketManager(ChatClientManager<ClientT>* clientMgr_):mbIsPacketRun(true)
    {
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

    bool PacketThread()
    {
        mPacketThread = thread(&PacketManager::Run, this);
        
        return true;
    }

    void Close()
    {
        mbIsPacketRun = false;
        mPacketThread.join();
        printf("[�˸�] PacketThread ����\n");
    }

    void ClassifyPacket(PacketInfo& packetInfo_)
    {
        PACKET_HEADER* packetHeader = (PACKET_HEADER*)packetInfo_.pPacketData;

        PROCESS_PACKET_FUNCTION fp = mPacketFunctionMap[packetHeader->Type];
        if(fp != NULL) (this->*fp)(packetInfo_);
    }


private:

    static unsigned __stdcall StaticPacketThread(void* arg)
    {
        PacketManager<ClientT>* This = (PacketManager<ClientT>*)arg;
        This->Run();
        return 0;
    };

    void            Run();

    void            EnqueuePacket(PacketInfo& packetInfo_);
    PacketInfo      DequeuePacket();


    void PROCESS_SERVER_ENTER(PacketInfo& packetInfo_);
    void PROCESS_CHAT_UNICAST(PacketInfo& packetInfo_);
    void PROCESS_SERVER_EXIT(PacketInfo& packetInfo_);
    void PROCESS_ROOM_ENTER(PacketInfo& packetInfo_);
    void PROCESS_ROOM_EXIT(PacketInfo& packetInfo_);
    void PROCESS_CHAT_BROADCAST(PacketInfo& packetInfo_);
    void PROCESS_CHAT_MULTICAST(PacketInfo& packetInfo_);
    

    bool                        mbIsPacketRun;
    thread                      mPacketThread;

    ChatClientManager<ClientT>* mClientMgr;
    std::queue<PacketInfo>      mPacketQueue;
    MAP_FUNC_LIST               mPacketFunctionMap;

    std::mutex                  mMutex;

};

#define __LOCKQUEUE     mMutex.lock();
#define __UNLOCKQUEUE   mMutex.unlock();

#endif

#include "PacketManager.hpp"