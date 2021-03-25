#pragma once
#include <windows.h>

enum CODE_ : UINT16 {
    UNINITIALIZED   = 0,

	SERVER_ENTER   	= 11,
	SERVER_EXIT    	= 12,

	ROOM_ENTER 	   	= 21,
	ROOM_EXIT		= 22,

	CHAT_BROADCAST 	= 31,
	CHAT_MULTICAST 	= 32,
	CHAT_UNICAST	= 33
};


#pragma pack(push, 1)


struct PACKET_HEADER{
    UINT16 Length;
    UINT16 Type;
};

const UINT32 PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);

const int MAX_NICKNAME_LEN = 32;
const int MAX_CONTENT_LEN = 1024;

typedef struct SYSTEM_PACKET : public PACKET_HEADER{
    char Sender[MAX_NICKNAME_LEN];
}ROOM_EXIT_PACKET, SERVER_ENTER_PACKET, SERVER_EXIT_PACKET;

struct ROOM_ENTER_PACKET : public SYSTEM_PACKET{
    int RoomNo;
};
const int ROOM_ENTER_PACKET_LENGTH = sizeof(ROOM_ENTER_PACKET);

typedef struct CHAT_PACKET : public PACKET_HEADER{
    char Sender[MAX_NICKNAME_LEN];
    char Content[MAX_CONTENT_LEN];
}MULTICAST_PACKET, BROADCAST_PACKET;

struct UNICAST_PACKET : public CHAT_PACKET{
    char Recver[MAX_NICKNAME_LEN];
};

#pragma pack(pop)