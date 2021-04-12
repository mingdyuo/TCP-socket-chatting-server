# TCP-socket-chatting-server

- 윈속 TCP를 사용하여 채팅 서버를 구현한 프로젝트입니다.

- 소켓 입출력 모델로는 IOCP를 사용하였습니다.

- 초기에 C++98로 작성하였다가, C++11로 수정하고 있는 과정입니다.

  (Visual Studio는 2005 버전에서 초기 작성하였고, 2019 버전으로 이동)

- IOCP 서버와 패킷 매니저의 구조는 [여기](https://github.com/mingdyuo/TCP-socket-chatting-server/blob/main/Documentation/IOCP%20%EC%B1%84%ED%8C%85%EC%84%9C%EB%B2%84%20%EB%8B%A8%EA%B3%84%EB%B3%84%20%EC%8B%A4%EC%8A%B5.md)를 참고했습니다.

- [여기](https://github.com/mingdyuo/TCP-socket-chatting-server/tree/main/Documentation)서 채팅서버 전체 제작 과정을 볼 수 있습니다.

- 클라이언트 단은 추후에 리팩토링을 할 예정입니다.

<br>

### How to build

- 필요한 조건

  1. Modern C++을 지원하는 Visual studio 
  2. Windows 환경 (IOCP는 MS 환경에서만 지원합니다.)

- 빌드 및 실행하기

  `git clone`을 받은 후 `TCP-socket-chatting-server/ChattingServer`에 있는 솔루션 파일인 `ChattingServer.sln`을 실행합니다.

  릴리즈 모드로 솔루션을 빌드합니다.

  `TCP-socket-chatting-server/ChattingServer/Release` 로 이동합니다.

  `ChattingServer.exe`를 먼저 실행한 다음 `ChattingClient.exe`를 실행합니다.

  부하 테스트를 하는 경우 `ChattingServer.exe`를 실행한 다음 `StressServer.exe`를 실행합니다.

<br>

## 서버

- IOCP 모델을 이용하여 만든 채팅 서버입니다.

- 클래스/구조체 상속 관계는 다음과 같습니다.

  1. `IOCPServer` ← `ChatServer`

     `IOCPServer`에서는 IOCP를 생성, 초기화하며 리슨 소켓을 활성화하고 스레드를 생성합니다.

  2. `ClientManager` ← `ChatClientManager`

     `ClientManager`를 상속받은 클래스는 `ClientInfo`를 상속받은 구조체를 이용할 수 있습니다.

  3. `ClientInfo` ← `UserInfo`

     `ClientInfo`는 클라이언트의 소켓 정보를 관리합니다.

     `UserInfo`는 클라이언트의 서버 기능 정보를 관리합니다. (닉네임, 채팅방 정보 등)

- 패킷 관련 자료구조는 다음과 같습니다.

  1. `enum eAction : UINT16 {}`

     클라이언트와 주고받는 패킷에 포함되어 있으며, 어떤 요청인지를 의미합니다.

  2. `enum eMessage : UINT32 { }`

     클라이언트와 주고받는 패킷 중 `SERVER_MESSAGE_PACKET`에 포함되는 값이며, 닉네임 생성 요청 시 서버의 응답을 의미합니다.

  3. `PACKET_HEADER`

     패킷의 종류와 길이 정보를 가지고 있습니다.

  4. `SYSTEM_PACKET`, `ROOM_ENTER_PACKET`, `CHAT_PACKET`, `UNICAST_PACKET`, `SERVER_MESSAGE_PACKET`

     `PACKET_HEADER`를 상속받으며, `Type`을 판별한 후 알맞은 패킷에 따라 캐스팅 합니다.

  5. `eSendType`

     `PacketManager`에서 사용하며, 해당 패킷을 어떤 방식으로 전송할 지를 가르킵니다.

  6. `PacketInfo`

     `PacketManager`에서 사용합니다.

     패킷을 송신한 클라이언트 정보, 패킷의 크기, 전송할 방식, 연결을 종료하는 패킷인지의 여부, 실제 패킷 내용을 가르키는 포인터가 있습니다.

     패킷 내용은 패킷 큐에 `Enqueue` 하기 전에 할당하며, `Dequeue`하고 전송을 완료한 후 해제합니다.

- 패킷 매니저에서는 `WSAsend()` 전용 스레드 역할을 하며, 패킷을 담는 큐를 관리합니다.

  1. `ChatServer`의 `WorkerThread`에서 패킷을 받으면 해당 패킷 `Type`에 따라 적절한 처리를 하고 패킷 매니저의 큐에 넣습니다.

  2. 패킷 매니저 클래스에서 독립된 스레드로 실행되는 `PacketManager::Run()`에서는 1번의 큐에서 패킷을 꺼내고, 전송 종류에 따라 알맞는 함수를 호출하여 전송합니다.

     ```c++
     switch (packetData.SendType) {
     case SENDTYPE_BROAD:
         mClientMgr->BroadCast(packetData);
         break;
     case SENDTYPE_MULTI:
         mClientMgr->MultiCast(packetData, packetData.isClose ? CLOSE : SEND);
         break;
     case SENDTYPE_UNI:
         mClientMgr->UniCast(packetData);
         break;
     default:
         break;
     }
     ```

<br>

## 스트레스 서버

- 채팅 서버의 부하를 테스트 하기 위해서 만들었습니다.
- IOCP 모델을 베이스로 해서 소켓 입출력을 처리하였습니다.
- [여기](https://github.com/mingdyuo/TCP-socket-chatting-server/blob/main/Documentation/TCP%20%EC%86%8C%EC%BC%93%20%EC%B1%84%ED%8C%85%20%EC%84%9C%EB%B2%84%20%EC%A0%9C%EC%9E%91%20-%204%20%EC%8A%A4%ED%8A%B8%EB%A0%88%EC%8A%A4%20%EC%84%9C%EB%B2%84.md)서 제작 과정을 볼 수 있습니다.
- 스트레스 서버 테스트를 하기 위해서는 `ChattingServer` 프로젝트의 `ChatClientManager.h`의 `int FindNickname(char*)` 함수에서 첫 줄에 있는 `return -1;`이 주석처리 되어 있지 않아야 합니다.

<br>

## 클라이언트

- 채팅을 주고받을 수 있는 프로그램입니다.

- 닉네임을 생성해야 하며, 로비 / 채팅방 시스템이 있습니다.

- 전체 채팅, 현재 속한 방 채팅, 귓속말 기능이 있습니다. 

- 현재 한글을 포함하여 긴 패킷을 보낼 시 문자열 길이 때문에 유니코드가 밀려서 이상한 문자가 출력되는 이슈가 있습니다.

  

