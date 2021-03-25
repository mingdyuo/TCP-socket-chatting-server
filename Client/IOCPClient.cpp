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
    
    SERVER_ENTER_PACKET* _packet = new SERVER_ENTER_PACKET();
    strncpy(_packet->Sender, mNickname, strlen(mNickname) + 1);
    _packet->Type = ROOM_ENTER;
    _packet->Length = (UINT16)strlen(mNickname) + 1;

    send(mSocket, (char*)_packet, _packet->Length + PACKET_HEADER_LENGTH, 0);
    delete _packet;

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
    printf("[�˸�] ������ ����Ǿ����ϴ�.\n");
    mbIsWorkerRun = false;
    closesocket(mSocket);
    return true;
}

void IOCPClient::processRecvMsg(char* received, char* content, char* sender){
    PACKET_HEADER* packetHeader = (PACKET_HEADER*)received;
    switch(packetHeader->Type){
        case SERVER_EXIT:
        case ROOM_ENTER:
        case ROOM_EXIT:
        {
            SYSTEM_PACKET* _packet = (SYSTEM_PACKET*)received;
            pos.RoomMessage(_packet->Sender, _packet->Type);
            break;
        }
        case CHAT_BROADCAST:
        case CHAT_MULTICAST:
        {
            CHAT_PACKET* _packet = (CHAT_PACKET*)received;
            memcpy(content, _packet->Content, _packet->Length);
            pos.Receive(_packet->Sender, content, _packet->Type);
            break;
        }
        case CHAT_UNICAST:
        {
            UNICAST_PACKET* _packet = (UNICAST_PACKET*)received;
        }
            
        default: break;
    }
}

CODE_ IOCPClient::processSendMsg(std::string& content){

    if(content == "/quit") {
        SERVER_EXIT_PACKET packet;
        packet.Type = SERVER_EXIT;
        strcpy(packet.Sender, mNickname);
        packet.Length = strlen(mNickname);

        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

        return SERVER_EXIT;
    }
    else if(content == "/lobby") {
        ROOM_EXIT_PACKET packet;
        packet.Type = ROOM_EXIT;
        strcpy(packet.Sender, mNickname);
        packet.Length = strlen(mNickname);

        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

        return ROOM_EXIT;
    }
    else if(content.find("/��ü ")==0) {
        content = content.substr(6);

        CHAT_PACKET packet;
        packet.Type = CHAT_BROADCAST;
        strcpy(packet.Sender, mNickname);
        strcpy(packet.Content, content.c_str());
        packet.Length = MAX_NICKNAME_LEN + content.length();

        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

        pos.Receive(mNickname, content.c_str(), CHAT_BROADCAST);

        return CHAT_BROADCAST;
    }
    else{
        CHAT_PACKET packet;
        packet.Type = CHAT_MULTICAST;
        strcpy(packet.Sender, mNickname);
        strcpy(packet.Content, content.c_str());
        packet.Length = MAX_NICKNAME_LEN + content.length();

        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}
        
        pos.Receive(mNickname, content.c_str(), CHAT_MULTICAST);

        return CHAT_MULTICAST;
    }

    return UNINITIALIZED;
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
            printf("������ ������ ���������ϴ�.\n");
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
    std::string content;
    
    while (mbIsWorkerRun) { 
        do{
            pos.SendBox(mNickname);
            getline(std::cin, content);
            if(!mbIsWorkerRun) return 0;
        } while(content.empty());

        CODE_ action = processSendMsg(content);

        if(action == UNINITIALIZED){
            Close();
            break;
        }

        content.clear();
    }
    return 0;
}
