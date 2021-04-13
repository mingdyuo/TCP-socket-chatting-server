### 항목

1. 클라이언트 단에서 ChatBox size를 넘는 문자열은 출력 과정에서 무한루프를 도는 현상 발생

2. `memcpy()`, `strcpy()`, `strncpy()`를 `std::copy()`, `std::copy_n()`로 변경하였음

3. `std::unique_ptr` 로 수정하였다.

   - GQCS에서 `stClientInfo`의 포인터 방식으로 새로운 I/O를 받아오는데 상관없나?
   - 로컬변수로 `unique_ptr`만들고 멤버 변수로 `move`한 후 dangling pointer가 되는데, 함수 빠져나오면 소멸되므로 상관 없나?

4. 리소스 보기

   서버 프로그램 그냥 켰을 때 메모리 13.2MB

   1000 client 14.8MB

   3000 client 21.1MB

