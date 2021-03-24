#ifndef _STRUCT_USER_INFO
#define _STRUCT_USER_INFO

#include <cstdio>
#include <windows.h>

struct stUserInfo
{
private:
    static const int LOBBY = 0;
public:
    static int id;
    stUserInfo(): mId(id++), mRoom(-1) 
    {ZeroMemory(mNickname, 32);}
    ~stUserInfo(){}

    char* GetNickname(){return mNickname;}
    void SetNickname(char* nickname){CopyMemory(mNickname, nickname, strlen(nickname));}
    
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