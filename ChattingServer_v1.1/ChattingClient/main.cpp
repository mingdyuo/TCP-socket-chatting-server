#include <iostream>
#include "Client.h"

void ErrorExit(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

int main()
{
	const int kServerPort = 9898;

	Client client;

	if (!client.Initialize(kServerPort))
		ErrorExit("�ʱ�ȭ ����");

	if (!client.ConnectToServer())
		ErrorExit("���� ���� ����");

	client.Close();

	printf("Ŭ���̾�Ʈ ����\n");
	getchar();

	return 0;
}
