#include <windows.h>

#ifndef _STRUCT_PACKET
#define _STRUCT_PACKET

enum eAction : UINT16 {
    UNINITIALIZED   = 0,

	SERVER_ENTER   	    = 11,
	SERVER_EXIT    	    = 12,

	ROOM_ENTER 	   	    = 21,
	ROOM_EXIT		    = 22,

	CHAT_BROADCAST 	    = 31,
	CHAT_MULTICAST 	    = 32,
	CHAT_UNICAST	    = 33,

    LIST_CURRENT_ROOM   = 41,
    LIST_ALL_ROOM       = 42,
    LIST_LOBBY          = 43,

    SERVER_MESSAGE      = 51,
};

enum eMessage : UINT32 {
    NICKNAME_ALREADY_EXIST  = 1,
    NICKNAME_CREATED,
    
    NICKNAME_NOT_FOUND,
    
};


#pragma pack(push, 1)


struct PACKET_HEADER{
    UINT16 Length;
    UINT16 Type;
};

const UINT32 PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);

const int MAX_NICKNAME_LEN = 32;
const int MAX_CONTENT_LEN = 900;

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

const int CHAT_PACKET_LENGTH = sizeof(CHAT_PACKET);

struct UNICAST_PACKET : public PACKET_HEADER{
    char Recver[MAX_NICKNAME_LEN];
    char Sender[MAX_NICKNAME_LEN];
    char Content[MAX_CONTENT_LEN];
};

const int UNICAST_BASE_LENGTH = MAX_NICKNAME_LEN * 2 + PACKET_HEADER_LENGTH;

struct SERVER_MESSAGE_PACKET : public PACKET_HEADER{
    UINT32 Message;
};

#pragma pack(pop)

#endif