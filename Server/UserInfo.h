#ifndef _STRUCT_USER_INFO
#define _STRUCT_USER_INFO

#include <cstdio>
#include <windows.h>

struct stUserInfo
{
public:
    static const int LOBBY = 0;
    static int id;
    stUserInfo(): mRoom(0) 
    {ZeroMemory(mNickname, 32);}
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

protected:
    int             mId;
    int             mRoom;    
    char            mNickname[32];
};


#endif