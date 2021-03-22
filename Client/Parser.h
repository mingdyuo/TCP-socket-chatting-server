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

struct Parser{
    int mAction;
    int mParsePos[2];
    char* mPacket;
    int mRecver;

    Parser(char* packet): mAction(UNINITIALIZED), mPacket(packet){
        int action = packet[0] - '0';
        action *= 10;
        action += packet[1] - '0';
        mAction = action;

        for(int i=0, cnt=0;cnt<2;i++){
            if(packet[i] == '|') mParsePos[cnt++] = i;
        }
    }

    int SetSender(char* nickname){
        int i = 0, nickIndex = mParsePos[1] + 1;
        for(;mPacket[i]!='\0';i++){
            mPacket[nickIndex + i] = nickname[i];
        }
        nickname[i] = '\0';

        return nickIndex + i;
    }

    void GetClient(char* nickname){
        int i = 0, nickIndex = mParsePos[1] + 1;
        for(;mPacket[i]!='\0';i++){
            nickname[i] = mPacket[nickIndex + i];
        }
        nickname[i] = '\0';
    }

    

};