#include "IOCPClient.h"
#include <conio.h>
#define MAX_SOCKBUF 1024
#define KEY_ESC 27

void ErrorHandling(char *message);
static const int SERVER_PORT = 9898;

 
int main()
{
    IOCPClient iocpClient;
    HANDLE sender, recver;
    iocpClient.InitSocket();
    iocpClient.ConnectServer(SERVER_PORT);
    iocpClient.SetNickname();
    iocpClient.CreateThreads(&sender, &recver);

    system("cls");
    printf("      ┏━━┓┏━━┓┏━━┓ ┏┓\n");
    printf("      ┗━┓┃┃┏┓┃┗━┓┃ ┃┃\n");
    printf("      ┏━┛┃┃┃┃┃┏━┛┃ ┃┃\n");
    printf("      ┃┏━┛┃┃┃┃┃┏━┛ ┃┃\n");
    printf("      ┃┗━┓┃┗┛┃┃┗━┓ ┃┃\n");
    printf("      ┗━━┛┗━━┛┗━━┛ ┗┛\n");
    printf("[알림] 대화방에 입장하셨습니다.\n", iocpClient.getNickname());


    iocpClient.ShowHelpBox();

    WaitForSingleObject (sender, INFINITE);
    WaitForSingleObject (recver, INFINITE);
    
    iocpClient.Close();

    printf("[알림] 클라이언트가 종료되었습니다. 아무키나 누르면 창을 종료합니다.\n");
    getchar();
    return 0;
}
 
