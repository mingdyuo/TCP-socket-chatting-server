
#ifndef _STRUCT_PACKET_INFO
#define _STRUCT_PACKET_INFO

#include <windows.h>

enum eSendType
{
    SENDTYPE_UNINTIALIZED = 0,
    SENDTYPE_MULTI = 1,
    SENDTYPE_UNI,
    SENDTYPE_BROAD
};

struct PacketInfo
{
    UINT32      ClientIndex;
    UINT32      DataSize;
    eSendType   SendType;
    bool        isClose;
    char*       pPacketData;

    PacketInfo(): DataSize(0), SendType(SENDTYPE_UNINTIALIZED), isClose(false), ClientIndex(0), pPacketData(nullptr) {}
    PacketInfo(const PacketInfo& origin):
        ClientIndex(origin.ClientIndex), 
        DataSize(origin.DataSize), 
        SendType(origin.SendType), 
        isClose(origin.isClose) 
        {
            pPacketData = new char[origin.DataSize];
		    CopyMemory(pPacketData, origin.pPacketData, DataSize);
        }

    void Set(UINT32 clinetIndex_, UINT32 dataSize_, char* pData_){
        ClientIndex = clinetIndex_;
        DataSize = dataSize_;
        pPacketData = pData_;
    }

    void Release(){
        delete pPacketData;
    }

};

#endif