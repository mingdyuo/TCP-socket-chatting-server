#include "Packet.h"
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

enum eNicknameMessage{
    TOO_LONG = 1,
    ALREADY_EXIST,
    TRIM_NEEDED,
    CREATE_SUCCESS
};

class Position{
private:
    COORD       mSendPos;
    COORD       mRecvPos;
    COORD       mHelpPos;
    COORD       mLobbyPos;

    int         mRoomPtr;
    static const int   mMaxRoomCount = 5;
    
    void initialize(){
        mSendPos.X = 0, mSendPos.Y = 20;
        mRecvPos.X = 0, mRecvPos.Y = 6;
        mHelpPos.X = 60, mHelpPos.Y = 7;
        mLobbyPos.X = 0, mLobbyPos.Y = 4;
    }
    
public:
    Position():mRoomPtr(1){initialize();}

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

    void NicknameBox(){
        system("cls");
        gotoxy(0,0); printf("����� �г����� �Է��ϼ���!\n");
        gotoxy(0,1); printf("�� \n"); gotoxy(3,1);
    }

    void NicknameErrorMessage(int error){
        gotoxy(0,2); 
        printf("%c[2K\n", 27);
        gotoxy(0,2); 
        switch(error){
            case TOO_LONG:
                printf("[!!!] �г����� �ʹ� ����!\n"); break;
            case ALREADY_EXIST:
                printf("[!!!] �̹� �����ϴ� �г����̿���!\n"); break;
            case TRIM_NEEDED:
                printf("[!!!] �г��ӿ��� ������ ���� �� �����!\n"); break;   
        }
        gotoxy(3,1); 
    }

    void Lobby(){
        mRoomPtr = 1;
        system("cls");
        gotoxy(mLobbyPos.X,0); printf("   / __      ___     // //  ___                       ___      __     //  ___   /  \n");
        gotoxy(mLobbyPos.X,1); printf("  //   ) ) //___) ) // // //   ) )     //  / /  / / //   ) ) //  ) ) // //   ) /   \n");
        gotoxy(mLobbyPos.X,2); printf(" //   / / //       // // //   / /     //  / /  / / //   / / //      // //   / /    \n");
        gotoxy(mLobbyPos.X,3); printf("//   / / ((____   // // ((___/ /     ((__( (__/ / ((___/ / //      // ((___/ /     \n");

        for(int i=1;i<=mMaxRoomCount;i++){
            gotoxy(mLobbyPos.X,mLobbyPos.Y + i); printf("[    ] %d�� ��ȭ�� \n", i);    
        }
        gotoxy(mLobbyPos.X + 2,mLobbyPos.Y + mRoomPtr); printf("��");
    }

    int LobbyControl(int input_){
        const int UP    = 72;
        const int DOWN  = 80;
        const int ENTER = 13;

        switch(input_){
            case ENTER:
                return mRoomPtr;
                break;
            case DOWN:
                if(mRoomPtr < mMaxRoomCount) {
                    gotoxy(mLobbyPos.X + 2,mLobbyPos.Y + mRoomPtr); printf("  ");
                    mRoomPtr++;
                    gotoxy(mLobbyPos.X + 2,mLobbyPos.Y + mRoomPtr); printf("��");
                }
                break;
            case UP:
                if(mRoomPtr > 1) {
                    gotoxy(mLobbyPos.X + 2,mLobbyPos.Y + mRoomPtr); printf("  ");
                    mRoomPtr--;
                    gotoxy(mLobbyPos.X + 2,mLobbyPos.Y + mRoomPtr); printf("��");
                }
                break;

        }
        return 0;
    }

    void EnterRoom(int pRoomNo, char* pNickname){
        initialize();
        system("cls");
        printf("      ������������������������ ����\n");
        printf("      ������������������������ ����\n");
        printf("      ������������������������ ����\n");
        printf("      ������������������������ ����\n");
        printf("      ������������������������ ����\n");
        printf("      ������������������������ ����\n");
        printf("[�˸�] %d�� ��ȭ�濡 �����ϼ̽��ϴ�.\n", pRoomNo);

        helpBox(pNickname);
        
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
        gotoxy(mHelpPos.X,mHelpPos.Y+0); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+1); printf("��                              ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+2); printf("��  +---------��ɾ�---------+  ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+3); printf("��   ��ü ä�� : /��ü �޼���   ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+4); printf("��   �ӼӸ� : /�г��� �޼���    ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+5); printf("��   �κ�� �̵� : /lobby       ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+6); printf("��   ä�� ���� : /quit          ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+7); printf("��                              ��\n");
        gotoxy(mHelpPos.X,mHelpPos.Y+8); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�\n");
        SendBox(nickname);
    }

    
    void SendBox(char* nickname){
        setColor(CHAT_MULTICAST);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
        printf("%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
        printf("===============================\n[%s��]: ", nickname);
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
        printf("[%s��]: %s\n", nickname, content);
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
            printf("[�˸�] %s���� �����ϼ̽��ϴ�.\n", nickname); break;
            case SERVER_EXIT:
            case ROOM_EXIT:
            printf("[�˸�] %s���� �����ϼ̽��ϴ�.\n", nickname); break;
            
        }
    }

    void ServerMessage(int Message){

        if(Message == NICKNAME_NOT_FOUND){
            ++mRecvPos.Y;
            if(mRecvPos.Y>=20) {
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mSendPos);
                setColor(CHAT_MULTICAST); 
                printf("%c[2K", 27);
                ++mSendPos.Y;
            }
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mRecvPos);
            setColor(CHAT_UNICAST);
            printf("[�˸�] �ش� ������ �������� �ʽ��ϴ�.\n"); 
        }

    }


};
