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
		ErrorExit("초기화 실패");

	if (!client.ConnectToServer())
		ErrorExit("서버 연결 실패");

	client.Close();

	printf("클라이언트 종료\n");
	getchar();

	return 0;
}
