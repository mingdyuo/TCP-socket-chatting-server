#include "IOCPClient.h"
#include <conio.h>
#define MAX_SOCKBUF 1024
#define KEY_ESC 27

static const int SERVER_PORT = 9898;

void ErrorExit(const char* msg){
    printf("%s\n", msg);
    exit(1);
}
 
int main()
{
    IOCPClient iocpClient;
    HANDLE sender, recver;
    bool bSuccess = true;

    bSuccess = iocpClient.InitSocket();
    if(!bSuccess) ErrorExit("소켓 초기화 실패");

    bSuccess = iocpClient.ConnectServer(SERVER_PORT);
    if(!bSuccess) ErrorExit("서버 연결 실패");

    bSuccess = iocpClient.SetNickname();
    if(!bSuccess) ErrorExit("닉네임 세팅 실패");

    bSuccess = iocpClient.CreateThreads(&sender, &recver);
    if(!bSuccess) ErrorExit("스레드 생성 실패");

    bSuccess = iocpClient.Lobby();
    if(!bSuccess) ErrorExit("로비 이동 실패");


    WaitForSingleObject (sender, INFINITE);
    WaitForSingleObject (recver, INFINITE);
    
    iocpClient.Close();

    printf("[알림] 클라이언트가 종료되었습니다. 엔터키를 눌러 창을 종료합니다.\n");
    getchar();
    return 0;
}
 
