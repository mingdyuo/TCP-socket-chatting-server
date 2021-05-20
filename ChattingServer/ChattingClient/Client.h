#pragma once

#include "ClientBase.h"
//#include "Display.h"
#include "../NetworkLib/PacketFactory.h"
#include <string>
#include <map>
#include <queue>
#include <mutex>

class Display;

enum class ClientState : int8_t
{
	PENDING = 0,
	LOGIN = 1,
	LOBBY = 2,
	CHATROOM = 3
};

namespace KeyInput
{
	constexpr int KEY_UP = 72;
	constexpr int KEY_DOWN = 80;
	constexpr int KEY_RIGHT = 77;
	constexpr int KEY_LEFT = 75;
	constexpr int KEY_ENTER = 13;
	constexpr int KEY_ESC = 27;
}

class Client : public ClientBase
{
public:
	bool Initialize(int bindPort);
	bool ServerEnter();

protected:
	virtual void SendThread();
	virtual void RecvThread();
	virtual void StateProcessThread();

	void LogicThread();

	void SendNickname();
	
	void RecvProcess(Packet* packet);
	void SendProcess(){}



	Packet*		PopQueue();
	void		PushQueue(Packet* packet);


private:
	static const int	kMaxNameLen = 32;
	std::string			nickname_;

	Display* display_;
	std::array<char, SOCKBUF_SIZE> recvBuf_;

	std::queue<Packet*> sendQueue_;
	std::queue<Packet*> recvQueue_;

	ClientState state_;

	std::mutex mutex_;

	std::thread jobThread_;

	uint32_t userId_;


};
