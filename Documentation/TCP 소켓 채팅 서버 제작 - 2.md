## 3월 마지막주 && 4월 첫째주

### 해야 하는 것들

- 패킷 매니저를 스레드로 하나 만들어서 처리하기 😎

  queue를 가지고 있으면서 거기에 Enque, deque, 패킷처리 😎

  패킷 매니저 스레드 하나에서만 send를 호출하도록 해보자 😎

- **데이터를 꽉 채운 큰 패킷 보냈을 때 처리하기**

  버퍼 오버런 학습

- 링버퍼 구현해보기

- Critical section 찾아서 lock 설정하기 

- `stUserinfo` & `stClientInfo` 클래스 구조 변경해보기 😎

- 함수 포인터를 mapping하여 사용해보기 😎

- accept 스레드를 사용하지 말고 `AcceptEx()`로 변경해보기

  대신 packet을 process하는 스레드를 새로 생성

- `vector`에서 메모리 재할당을 사용해보자 → `resize()` 😎

- 스레드 생성 함수를 `_beginthreadex`로 변경해보자 😎

- TCP 연결 종료에 대해 좀더 알아보자

### 기타 조언

- 어떤 함수/API의 기능을 알고 어떤 장점이 있는지 알고 사용하자.

  예를 들면 `accept()`와 `AcceptEx()`는 작은 테스트에서는 차이가 티가 나지 않는다.

  왜 좋은지 확실히 알고 사용해야 한다.

- 자기 라이브러리를 하나 만들어서 쓸 수 있으면 좋다

- Stress test server tool 만들기는 기본적인 것이라고 한다 (이번주에는 여유가 되는 경우에만 ㅇㅇ)

- `memcpy()`를 많이 사용하는 것은 좋지 않다

- `wsasend()`를 죄다 보내면, 다 대기 상태로 가서 아무도 못보낼 것이다.

  그 내부적인 동작을 알고 동시에 여러 개 보내는 경우를 해결해봐라

- Boost asio, 상용소스와 같이 완성된 네트워크 프레임워크는 예외 로직등으로 그 양이 엄청 많아서 처음부터 이해하기가 어렵다. 하나씩 단계적으로 기능을 쌓아가고, 모듈화 해보면서 이해해보는 것을 추천.



### 03월 29일 월요일

- 클래스 구조 변경해보기

  `template` 사용해서 `ClientManager`에서는 `ClientInfo`를 상속한 어떤 구조체든지 사용할 수 있도록

### 03월 30일 화요일

- `mClientInfos`라는 클라이언트 저장 벡터의 크기를 서버 시작 시 `maxClientCount`의 4분의 1만 생성하도록 함. 그리고 클라이언트가 그것보다 더 많이 들어오면 새로 생성.

  생성 개수 순서는 1/4, 1/2, 1배 이렇게 생성된다.

- send하는 부분에서 `new`, `delete`의 동적 할당을 하므로 스레드 함수를 `CreateThread`에서 `_beginthreadex`로 변경

### 03월 31일 수요일

- Packet Manager 클래스 생성, 단일로 send 만 하는 역할 하는 스레드로 작동
- Packet Info 구조체 만들어서 관리
- 패킷 매니저에서 각 패킷의 유저 행동 종류에 따라서 다른 함수로 처리하도록 매핑

### 04월 01일 목요일

- Packet Manager 헤더와 템플릿 멤버함수 분리
- 학습 : `AcceptEx`, TCP 윈속2 버퍼 구조, Locked page, non-paged pool

### 04월 02일 금요일

- 큰 패킷 클라이언트 단에서 쪼개서 보내기 

- 문제 : 받을 때 한글이 이상하게 뜬다 근데 잘 됬다가 잘 안됬다가 왔다갔다함

  ![image](https://user-images.githubusercontent.com/41130448/113376265-410d9800-93ac-11eb-9ce9-265fdc03bcd9.png)

- 문제 : 늦게 보낸게 먼저 전송됨 (아마 패킷 classify하고 처리하는 과정에서 크기 때문에 속도차가 나기 때문이 아닐까,,,)

  ![image](https://user-images.githubusercontent.com/41130448/113375980-9f864680-93ab-11eb-8428-f5f4d239cbc2.png)

