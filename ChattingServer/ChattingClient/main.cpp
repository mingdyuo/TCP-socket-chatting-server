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
    if(!bSuccess) ErrorExit("���� �ʱ�ȭ ����");

    bSuccess = iocpClient.ConnectServer(SERVER_PORT);
    if(!bSuccess) ErrorExit("���� ���� ����");

    bSuccess = iocpClient.SetNickname();
    if(!bSuccess) ErrorExit("�г��� ���� ����");

    bSuccess = iocpClient.CreateThreads(&sender, &recver);
    if(!bSuccess) ErrorExit("������ ���� ����");

    bSuccess = iocpClient.Lobby();
    if(!bSuccess) ErrorExit("�κ� �̵� ����");


    WaitForSingleObject (sender, INFINITE);
    WaitForSingleObject (recver, INFINITE);
    
    iocpClient.Close();

    printf("[�˸�] Ŭ���̾�Ʈ�� ����Ǿ����ϴ�. ����Ű�� ���� â�� �����մϴ�.\n");
    getchar();
    return 0;
}
 
