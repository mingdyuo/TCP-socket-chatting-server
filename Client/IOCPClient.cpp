#include "IOCPClient.h"


bool IOCPClient::InitSocket(){
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }

    mSocket = socket(PF_INET, SOCK_STREAM, 0); 

    if (mSocket == INVALID_SOCKET){
        printf("[����] socket()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }
        
    printf("[����] ���� �ʱ�ȭ �Ϸ�\n");
    return true;
}

bool IOCPClient::ConnectServer(int bBindPort){
    SOCKADDR_IN serverAddress;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serverAddress.sin_port = htons(bBindPort); 

    if (SOCKET_ERROR == connect(mSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress))){
        printf("[����] connect()�Լ� ���� : %d\n", WSAGetLastError());
        return false;
    }

    printf("[����] connect �Ϸ�\n");
    return true;
}

bool IOCPClient::SetNickname(){
    do {
        printf("[�����] ��ȭ�� ����� �г����� �Է��ϼ���(�ִ� 31����)\n");
        scanf("%s", mNickname);
    } while(strlen(mNickname)==0);
    char packet[36] = "11||";
    strcat(packet, mNickname);
    send(mSocket, packet, (int)strlen(packet), 0);
    return true;
}

bool IOCPClient::CreateThreads(HANDLE* sender, HANDLE* recver){
    *recver = CreateThread(NULL, 0, StaticRecvThread, this, 0, 0);
    *sender = CreateThread(NULL, 0, StaticSendThread, this, 0, 0);
    printf("[����] ������ ���� �Ϸ�\n");

    Sleep(200);
    return true;
}

void IOCPClient::ShowHelpBox(){
    pos.helpBox(mNickname);
}

bool IOCPClient::Close(){
    printf("[�˶�] ������ ����Ǿ����ϴ�.\n");
    mbIsWorkerRun = false;
    closesocket(mSocket);
    return true;
}

void IOCPClient::parseContent(char* received, char* content, char* sender){
    int idx = 3, nickIdx = 0;
    for(;received[idx]!='|';++idx){
        content[idx - 3] = received[idx];
    }
    content[idx - 3] = '\0';
    ++idx;
    for(;received[idx+nickIdx]!='\0';++nickIdx){
        sender[nickIdx] = received[idx+nickIdx];
    }
    sender[nickIdx] = '\0';
}

void IOCPClient::processRecvMsg(char* received, char* content, char* sender){
    int action = received[0] - '0';
    action *= 10;
    action += received[1] - '0';

    switch(action){
        case SERVER_EXIT:
        case ROOM_ENTER:
        case ROOM_EXIT:
            memcpy(sender, &received[4], sizeof(received) - 4);
            pos.RoomMessage(sender, action);
            break;
        case CHAT_BROADCAST:
        case CHAT_MULTICAST:
        case CHAT_UNICAST:
            parseContent(received, content, sender);
            pos.Receive(sender, content, action); break;
        default:
            break;
    }
}

int IOCPClient::processSendMsg(std::string& content, std::string& packet){
    if(content[0]!='/') {
        packet = "32|" + content + '|' + mNickname;
        return CHAT_MULTICAST;
    }
    else if(content == "/quit") {
        packet = "12||" + std::string(mNickname);
        return SERVER_EXIT;
    }
    else if(content == "/lobby") {
        packet = "22||" + std::string(mNickname);
        return ROOM_EXIT;
    }
    else if(content.find("/��ü ")==0) {
        content = content.substr(6);
        packet = "31|" + content + '|' + std::string(mNickname);
        return CHAT_BROADCAST;
    }
    else{
        packet = "32|" + content + '|' + mNickname;
        return CHAT_MULTICAST;
    }

    // �ӼӸ� ó�� �߰� �ؾ� ��
}


DWORD IOCPClient::RecvThread(){
    char received[1024];
    char content[1024];
    char sender[32];
    while (mbIsWorkerRun) { 
        memset(&received, 0, sizeof(received));
        int length = recv(mSocket, received, sizeof(received), 0);
        if(length <= 0) {
            printf("������ ���������ϴ�.\n");
            Close();
            break;
        }
        received[length] = '\0';
        
        processRecvMsg(received, content, sender);
        pos.SendBox(mNickname);
    }
    return 0;
}

DWORD IOCPClient::SendThread(){
    std::string content, packet;
    while (mbIsWorkerRun) { 
        do{
            pos.SendBox(mNickname);
            getline(std::cin, content);
            if(!mbIsWorkerRun) return 0;
        } while(content.empty());

        int action = processSendMsg(content, packet);
        switch(action){
            case SERVER_EXIT:
                send(mSocket, packet.c_str(), (int)packet.length(), 0);
                Close();
                return 0;
            case ROOM_EXIT:
            {
                int success = send(mSocket, packet.c_str(), (int)packet.length(), 0);
                if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return 0;}
                break;
            }
            case CHAT_BROADCAST:
            case CHAT_MULTICAST:
            case CHAT_UNICAST:
            {
                int success = send(mSocket, packet.c_str(), (int)packet.length(), 0);
                if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return 0;}
                pos.Receive(mNickname, content.c_str(), action);
            }
            default:
                break;
        }
        content.clear();
    }
    return 0;
}
