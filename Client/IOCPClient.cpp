#include "IOCPClient.h"
#include "conio.h"

bool IOCPClient::InitSocket(){
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
        return false;
    }

    mSocket = socket(PF_INET, SOCK_STREAM, 0); 

    if (mSocket == INVALID_SOCKET){
        printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
        return false;
    }
        
    printf("[성공] 소켓 초기화 완료\n");
    return true;
}

bool IOCPClient::ConnectServer(int bBindPort){
    SOCKADDR_IN serverAddress;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serverAddress.sin_port = htons(bBindPort); 

    if (SOCKET_ERROR == connect(mSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress))){
        printf("[에러] connect()함수 실패 : %d\n", WSAGetLastError());
        return false;
    }

    printf("[성공] connect 완료\n");
    return true;
}

int IOCPClient::NicknameCheck(const char* nickname){
    char recved[15];
    if(strlen(nickname)>=31) {
        return TOO_LONG;
    }
    for(int i=0;i<strlen(nickname);i++){
        if(nickname[i] == ' ') return TRIM_NEEDED;
    }

    strcpy(mNickname, nickname);

    SERVER_ENTER_PACKET _packet;
    strncpy(_packet.Sender, mNickname, strlen(mNickname) + 1);
    _packet.Type = SERVER_ENTER;
    _packet.Length = (UINT16)strlen(mNickname) + 1;

    int success = send(mSocket, (char*)&_packet, _packet.Length + PACKET_HEADER_LENGTH, 0);
    if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return 0;}

    int length = recv(mSocket, recved, sizeof(recved), 0);
    if(length <= 0) {
        printf("서버와 연결이 끊어졌습니다.\n");
        Close();
        return 0;
    }
    recved[length] = '\0';

    SERVER_MESSAGE_PACKET* recvPacket = (SERVER_MESSAGE_PACKET*)recved;
    if(recvPacket->Message == NICKNAME_ALREADY_EXIST) return ALREADY_EXIST;

    return CREATE_SUCCESS;
}

bool IOCPClient::SetNickname(){
    
    int errorMsg = -1;
    std::string nickname;
    while(true){
        pos.NicknameBox();
        if(errorMsg > -1) pos.NicknameErrorMessage(errorMsg);
        getline(std::cin, nickname);
        if (nickname.empty()) continue;
        errorMsg = NicknameCheck(nickname.c_str());
        if (errorMsg == CREATE_SUCCESS) break;
        if (errorMsg == 0) return false;
        nickname.clear();
    }

    return true;
}

bool IOCPClient::CreateThreads(HANDLE* sender, HANDLE* recver){
    *recver = CreateThread(NULL, 0, StaticRecvThread, this, 0, 0);
    *sender = CreateThread(NULL, 0, StaticSendThread, this, 0, 0);
    printf("[성공] 스레드 생성 완료\n");

    Sleep(200);
    return true;
}

bool IOCPClient::Lobby(){
    pos.Lobby();
    const int UP    = 72;
    const int DOWN  = 80;
    const int ENTER = 13;

    int key;
    int roomNo = -1;
    while(roomNo < 0){
        key = getch(); 
        if(key==224){ 
            key=getch();
            switch(key){
                case DOWN:  
                    pos.LobbyControl(DOWN);
                    break;
                case UP:  
                    pos.LobbyControl(UP);
                    break;
                case ENTER:
                    roomNo = pos.LobbyControl(ENTER);
                    break;
            }
        }
        else if (key == ENTER){
            roomNo = pos.LobbyControl(ENTER);
        }
    }

    ROOM_ENTER_PACKET packet;
    packet.Type = ROOM_ENTER;
    packet.RoomNo = roomNo;
    strcpy(packet.Sender, mNickname);
    packet.Length = strlen(mNickname);

    int success = send(mSocket, (char*)&packet, ROOM_ENTER_PACKET_LENGTH, 0);
    if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

    mMutex.Lock();
    pos.EnterRoom(roomNo, mNickname);
    mMutex.Unlock();
    mRoom = roomNo;
    
    return true;
}



bool IOCPClient::Close(){
    printf("[알림] 연결이 종료되었습니다.\n");
    mbIsWorkerRun = false;
    closesocket(mSocket);
    return true;
}

void IOCPClient::processRecvMsg(char* received, char* content, char* sender){
    PACKET_HEADER* packetHeader = (PACKET_HEADER*)received;
    mMutex.Lock();
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
            memcpy(content, _packet->Content, strlen(_packet->Content) + 1);
            pos.Receive(_packet->Sender, content, _packet->Type);
            break;
        }
        case SERVER_MESSAGE:
        {   
            SERVER_MESSAGE_PACKET* _packet = (SERVER_MESSAGE_PACKET*)received;
            pos.ServerMessage(_packet->Message);
            break;
        }
            
        default: break;
    }
    mMutex.Unlock();
}

eAction IOCPClient::processSendMsg(std::string& content){

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

        mRoom = 0;
        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

        return ROOM_EXIT;
    }
    else if(content.find("/전체 ")==0) {
        content = content.substr(6);

        CHAT_PACKET packet;
        packet.Type = CHAT_BROADCAST;
        strcpy(packet.Sender, mNickname);
        strcpy(packet.Content, content.c_str());
        packet.Length = MAX_NICKNAME_LEN + content.length();

        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

        return CHAT_BROADCAST;
    }
    else if (content[0] == '/'){
        std::string recver = content.substr(1, content.find(' ') - 1);
        content = content.substr(content.find(' ') + 1);
        UNICAST_PACKET packet;
        packet.Type = CHAT_UNICAST;
        strcpy(packet.Sender, mNickname);
        strcpy(packet.Content, content.c_str());
        strcpy(packet.Recver, recver.c_str());
        packet.Length = strlen(content.c_str());

        int success = send(mSocket, (char*)&packet, UNICAST_BASE_LENGTH + packet.Length, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}

        return CHAT_UNICAST;
    }
    else{
        CHAT_PACKET packet;
        packet.Type = CHAT_MULTICAST;
        strcpy(packet.Sender, mNickname);
        strcpy(packet.Content, content.c_str());
        packet.Length = MAX_NICKNAME_LEN + content.length();

        int success = send(mSocket, (char*)&packet, packet.Length + PACKET_HEADER_LENGTH, 0);
        if(success == SOCKET_ERROR) {mbIsWorkerRun = false; return UNINITIALIZED;}
        
        return CHAT_MULTICAST;
    }

    return UNINITIALIZED;
}


DWORD IOCPClient::RecvThread(){
    char received[1024];
    char content[1024];
    char sender[32];
    while (mbIsWorkerRun) { 
        while(mRoom == 0) Sleep(500);
        memset(&received, 0, sizeof(received));
        int length = recv(mSocket, received, sizeof(received), 0);
        if(length <= 0) {
            printf("서버와 연결이 끊어졌습니다.\n");
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
        while(mRoom == 0) Sleep(500);
        do{
            mMutex.Lock();
            pos.SendBox(mNickname);
            mMutex.Unlock();
            getline(std::cin, content);
            if(!mbIsWorkerRun) return 0;
        } while(content.empty());

        eAction action = processSendMsg(content);

        if(action == UNINITIALIZED){        
            Close();
            break;
        }
        else if(action == ROOM_EXIT){
            Lobby();
        }

        content.clear();
    }
    return 0;
}
