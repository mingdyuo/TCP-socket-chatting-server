#include <windows.h>
#include "parser.h"
#define BLACK 0 
#define BLUE 1 
#define GREEN 2 
#define CYAN 3 
#define RED 4 
#define MAGENTA 5 
#define BROWN 6 
#define LIGHTGRAY 7 
#define DARKGRAY 8 
#define LIGHTBLUE 9 
#define LIGHTGREEN 10 
#define LIGHTCYAN 11 
#define LIGHTRED 12 
#define LIGHTMAGENTA 13 
#define YELLOW 14 
#define WHITE 15 

enum IOOperation
{
	RECV,
	SEND
};

class Position{
private:
    COORD mSendPos;
    COORD mRecvPos;
    COORD mHelpPos;
public:
    Position(){
        mSendPos.X = 0, mSendPos.Y = 20;
        mRecvPos.X = 0, mRecvPos.Y = 1;
        mHelpPos.X = 60, mHelpPos.Y = 7;
    }
    
    void setColor(int action){
        int color;
        switch(action){
            case CHAT_MULTICAST: 
                color=WHITE+BLACK*16; break;
            case CHAT_UNICAST: 
                color=GREEN+BLACK*16; break;
            case CHAT_BROADCAST: 
                color=RED+YELLOW*16; break;
            case SERVER_EXIT:
            case ROOM_ENTER: 
            case ROOM_EXIT:
                color=WHITE+BROWN*16; break;
            default:
                color=WHITE+BLACK*16; break;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); 
    }


    void SetSendPos(int nicknameLength){
        mSendPos.X = nicknameLength;
    }

    void gotoxy(int x,int y) {
        COORD pos={x,y};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
    }

    void helpBox(char* nickname){
        setColor(CHAT_MULTICAST);
        gotoxy(mHelpPos.X,mHelpPos.Y+0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+1); printf("▤                              ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+2); printf("▤  +---------명령어---------+  ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+3); printf("▤   전체 채팅 : /전체 메세지   ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+4); printf("▤   귓속말 : /닉네임 메세지    ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+5); printf("▤   로비로 이동 : /lobby       ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+6); printf("▤   채팅 종료 : /quit          ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+7); printf("▤                              ▤\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+8); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤\n");
        SendBox(nickname);
    }

    
    void SendBox(char* nickname){
        setColor(CHAT_MULTICAST);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
        printf("%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
        printf("===============================\n[%s님]: ", nickname);
    }

    void Receive(char* nickname, const char* content, int action){
        ++mRecvPos.Y;
        if(mRecvPos.Y>=20) {
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
            setColor(CHAT_MULTICAST);
            printf("%c[2K", 27);
            ++mSendPos.Y;
        }
        setColor(action);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mRecvPos);
        printf("[%s님]: %s\n", nickname, content);
    }

    void RoomMessage(char* nickname, int action){
        ++mRecvPos.Y;
        if(mRecvPos.Y>=20) {
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
            setColor(CHAT_MULTICAST); 
            printf("%c[2K", 27);
            ++mSendPos.Y;
        }
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mRecvPos);
        setColor(action);
        switch(action){
            case ROOM_ENTER:
            printf("[%s님이 입장하셨습니다.]\n", nickname); break;
            case SERVER_EXIT:
            case ROOM_EXIT:
            printf("[%s님이 퇴장하셨습니다.]\n", nickname); break;
            
        }
    }


};
