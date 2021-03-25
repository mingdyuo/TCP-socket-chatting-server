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
    iocpClient.Lobby();


    WaitForSingleObject (sender, INFINITE);
    WaitForSingleObject (recver, INFINITE);
    
    iocpClient.Close();

    printf("[�˸�] Ŭ���̾�Ʈ�� ����Ǿ����ϴ�. ����Ű�� ���� â�� �����մϴ�.\n");
    getchar();
    return 0;
}
 
