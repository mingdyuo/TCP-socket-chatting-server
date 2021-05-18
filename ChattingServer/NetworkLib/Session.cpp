#include "Session.h"

void Session::Close()
{
    if (false == IsConnected())
        return;

    struct linger stLinger = { 0, 0 };
    shutdown(socket_, SD_BOTH);
    setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
    closesocket(socket_);

}

bool Session::BindIOCompletionPort(HANDLE iocpHandle)
{
    HANDLE hIOCP = CreateIoCompletionPort(
        (HANDLE)socket_,
        iocpHandle,
        (ULONG_PTR)(this),
        0
    );

    if (hIOCP == INVALID_HANDLE_VALUE)
    {
        printf("[ERROR] CreateIoCompletionPort() function: %d\n", GetLastError());
        return false;
    }

    return true;
}

bool Session::Connect(HANDLE iocpHandle, SOCKET socket)
{
    socket_ = socket;
    if (BindIOCompletionPort(iocpHandle) == false)
    {
        printf("[ERROR] BindIOCompletionPort() funtion\n");
        return false;
    }

    if (false == BindRecv())
    {
        Close();
        return false;
    }

    printf("[SUCCESS] CLIENT(%d) CREATED\n", userId_);

    return true;
}

int Session::recv(WSABUF wsabuf)
{
    DWORD dwFlag = 0;
    DWORD dwRecvNumBytes = 0;

    return WSARecv(
        socket_,
        &wsabuf,
        1,
        &dwRecvNumBytes,
        &dwFlag,
        (LPWSAOVERLAPPED)&IoData_[IO_RECV],
        NULL
    );
}


bool Session::BindRecv()
{
    DWORD dwFlag = 0;
    DWORD dwRecvNumBytes = 0;

    int nRet = this->recv(IoData_[IO_RECV].WsaBuf());

    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
    {
        printf("[RECV_SOCKET_ERROR] Client Connection Terminated\n");
        Close();
        return false;
    }

    return true;
}


Packet* Session::OnRecv(size_t ioSize)
{
    recvBuffer_.fill(0);
    memcpy_s(recvBuffer_.data(), SOCKBUF_SIZE, IoData_[IO_RECV].buffer_.data(), ioSize);

    Packet* packet = nullptr;
    packet = PacketFactory::GetPacket(recvBuffer_.data(), ioSize);

    if (!packet)
    {
        printf("[ERROR] client(%d) invalid packet\n", userId_);
        return nullptr;
    }

    printf("[RECV] PacketType : %d\n", packet->type());

    return packet;
}

bool Session::SendPacket(Packet* const packet)
{
    Stream stream;
    packet->encode(stream);

    IoData* ioData = new IoData(IO_SEND);

    WSABUF wsabuf = ioData->SetData(stream);

    DWORD dwSentNumBytes = 0;

    int nRet = WSASend(socket_,
        &wsabuf,
        1,
        &dwSentNumBytes,
        0,
        (LPWSAOVERLAPPED)ioData,
        NULL
    );

    printf("[sentByte] %d\n", dwSentNumBytes);

    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
    {
        printf("[ERROR] client(%d) Send Error\n", userId_);
        return false;
    }

    return true;
}

int Session::send(WSABUF wsabuf)
{
    DWORD dwSentNumBytes = 0;

    return WSASend(socket_,
        &wsabuf,
        1,
        &dwSentNumBytes,
        0,
        (LPWSAOVERLAPPED)&IoData_[IO_SEND],
        NULL
    );
}