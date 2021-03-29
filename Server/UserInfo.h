
#include "ClientInfo.h"
#include <windows.h>

#ifndef _STRUCT_USER_INFO
#define _STRUCT_USER_INFO

struct stUserInfo : public stClientInfo
{
public:
    static const int LOBBY = 0;
    static int id;
    stUserInfo(): mRoom(0), stClientInfo(-1) {ZeroMemory(mNickname, sizeof(mNickname));}
    stUserInfo(UINT32 index): mRoom(0), stClientInfo(index) {ZeroMemory(mNickname, sizeof(mNickname));}
    ~stUserInfo(){}
    

    char* GetNickname(){return mNickname;}
    void SetNickname(char* nickname){
        mRoom = 0;
        mId = id++;
        CopyMemory(mNickname, nickname, strlen(nickname));
    }

    int GetRoom(){return mRoom;}
    
    bool EnterRoom(int RoomNumber){
        if(mRoom!=LOBBY) return false;
        mRoom = RoomNumber;
        return true;
    }
    bool ExitRoom(){
        if(mRoom==LOBBY) return false;
        mRoom = LOBBY;
        return true;
    }

private:
    int             mId;
    int             mRoom;    
    char            mNickname[32];
};

#endif