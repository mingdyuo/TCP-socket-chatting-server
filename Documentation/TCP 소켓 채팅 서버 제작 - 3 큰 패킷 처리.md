## 큰 패킷 처리하기

#### 04월 02일 금요일

### 문제 1 : 늦게 보낸 게 먼저 전송됨

- 아마 패킷을 classify하고 처리하는 과정에서 패킷의 크기 때문에 속도차가 나는 게 아닐까 싶음

- 이거 해결하려면 클라이언트마다 각각 큐를 두거나 해야 하지 않을까?

  ![image](https://user-images.githubusercontent.com/41130448/113375980-9f864680-93ab-11eb-8428-f5f4d239cbc2.png)

<br>

### 문제 2 : 긴 패킷을 받을 때 한글이 이상하게 뜬다

- 근데 잘 됬다가 잘 안됬다가 왔다갔다 한다.

- 지금은 여러줄이 죄다 변형되지는 않고 일부만 변형됨

  ![image](https://user-images.githubusercontent.com/41130448/113376265-410d9800-93ac-11eb-9ce9-265fdc03bcd9.png)

<br>

### 문제 3 : 서버로 보낼 때에도 일부가 깨짐

- 큰 패킷 처리... 대체 어떻게 해야 하는 것일까

- Client Side

  ![image](https://user-images.githubusercontent.com/41130448/113401956-3caea280-93df-11eb-8476-72653444985e.png)

- Server Side

  ![image](https://user-images.githubusercontent.com/41130448/113402213-9fa03980-93df-11eb-8d27-fd3d07d04370.png)

<br>

### 문제 4 : 많은 클라이언트 감당할 수 있는지 테스트 못함

- 일단 단순 접속은 잘 된다. 500개 클라이언트 접속을 시도했는데 연결은 된다. 

  하지만 채팅도 원활히 되는지는 테스트 해보지 못했음


<br>

### 한글 깨짐 해결하기

#### 04월 06일 화요일

받은 데이터에서 강제로 중간에 개행을 하기 위해 다음과 같은 방식으로 출력을 해보았다.

```c++
for(int i=0;i<strlen(str);i++){
	printf("%c", str[i]);
}
```

근데 알고보니 한글은 2byte, 영어 숫자는 1byte로 되어 있어서 g