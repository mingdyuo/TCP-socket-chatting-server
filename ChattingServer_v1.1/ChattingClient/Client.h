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
	typedef void (Client::* PROCESS_FUNCTION)(Packet* packet);
	typedef std::map<packet_header_size, PROCESS_FUNCTION> PROCESS_FUNCTION_MAP;

public:
	Client();
	~Client();

	bool Initialize(int bindPort);
	bool ConnectToServer();

protected:
	virtual void	SendThread();
	virtual void	RecvThread();
	virtual void	StateProcessThread();
	void			LogicThread();


	void SendNickname();
	void LobbyPage();
	
	void RecvProcess(Packet* packet);
	void StateProcess();


	void		PushQueue(Packet* packet);
	Packet*		PopQueue();

private:
	void F_SERVER_ENTER_OK(Packet* packet);
	void F_LOBBY_ROOM_INFO(Packet* packet);
	void F_LOBBY_USER_INFO(Packet* packet);


private:
	static const int	kMaxNameLen = 32;
	std::string			nickname_;

	uint32_t			userId_;
	ClientState			state_;

	Display*			display_;

private:
	std::array<char, SOCKBUF_SIZE> recvBuf_;

	std::queue<Packet*> sendQueue_;
	std::queue<Packet*> recvQueue_;


	std::thread			jobThread_;
	std::mutex			mutex_;

	PROCESS_FUNCTION_MAP processFunc_;

};
