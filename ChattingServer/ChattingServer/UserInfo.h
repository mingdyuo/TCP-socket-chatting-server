
#include "ClientInfo.h"
#include <windows.h>

#ifndef _STRUCT_USER_INFO
#define _STRUCT_USER_INFO

struct stUserInfo : public stClientInfo
{
public:
    static const int LOBBY = 0;
    static int id;
    stUserInfo(): mRoom(LOBBY), mPrevRoom(LOBBY), stClientInfo(-1), mId(-1) {ZeroMemory(mNickname, sizeof(mNickname));}
    stUserInfo(UINT32 index): mRoom(LOBBY), mPrevRoom(LOBBY), mId(-1), stClientInfo(index) {ZeroMemory(mNickname, sizeof(mNickname));}
    ~stUserInfo(){}
    

    char* GetNickname(){return mNickname;}
    void SetNickname(char* nickname){
        mRoom = 0;
        mId = id++;
        CopyMemory(mNickname, nickname, strlen(nickname));
    }

    int GetRoom(){return mRoom;}
    int GetPrevRoom(){return mPrevRoom;}
    
    bool EnterRoom(int RoomNumber){
        if(mRoom!=LOBBY) return false;
        mRoom = RoomNumber;
        return true;
    }
    bool ExitRoom(){
        if(mRoom==LOBBY) return false;
        mPrevRoom   = mRoom;
        mRoom       = LOBBY;
        return true;
    }

private:
    int             mId;
    int             mRoom;    
    int             mPrevRoom;
    char            mNickname[32];
};

#endif