#pragma once

enum Action {
    UNINITIALIZED   = 0,

	SERVER_ENTER   	= 11,
	SERVER_EXIT    	= 12,

	ROOM_ENTER 	   	= 21,
	ROOM_EXIT		= 22,

	CHAT_BROADCAST 	= 31,
	CHAT_MULTICAST 	= 32,
	CHAT_UNICAST	= 33
};
