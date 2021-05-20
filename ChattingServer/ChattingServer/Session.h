#ifndef SESSION_H
#define SESSION_H

#include "IoData.h"
#include "PacketFactory.h"
#include <array>

constexpr int IO_DATA_SIZE = 2;

struct Session
{
	using Byte = unsigned char;
public:
	Session(uint32_t pid) :
		socket_(INVALID_SOCKET), userId_(pid), IoData_({ IO_RECV, IO_SEND })
	{ }

	Session(Session const&) = delete;
	Session(Session const&&) = delete;
	Session& operator=(Session const&) = delete;
	Session& operator=(Session&&) = delete;

	~Session() {}


	void Close();

	bool IsConnected() const { return socket_ != INVALID_SOCKET; }
	uint32_t GetId() const { return userId_; }

	bool Connect(HANDLE iocpHandle, SOCKET socket);
	bool BindRecv();

	Packet* OnRecv(size_t ioSize);
	bool SendPacket(Packet* packet);
	//bool SendPacket(Packet& packet);

private:
	int recv(WSABUF wsabuf);
	int send(WSABUF wsabuf);

	bool BindIOCompletionPort(HANDLE iocpHandle);

protected:
	SOCKET			socket_;
	std::array<IoData, IO_DATA_SIZE>	IoData_;
	std::array<char, SOCKBUF_SIZE>		recvBuffer_;

private:
	uint32_t							userId_;
};

#endif
