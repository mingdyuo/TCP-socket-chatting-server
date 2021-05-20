#include "Client.h"
#include "LoginDisplay.h"

/* * * * * * * * * * * 
*   Initialize & Start
* * * * * * * * * * * */

Client::Client() 
{
	processFunc_[E_PK_S_SERVER_ENTER_OK] = &Client::F_SERVER_ENTER_OK;
	
}

Client::~Client()
{
	//< LOCK
	while (!sendQueue_.empty())
	{
		Packet* packet = sendQueue_.front();
		sendQueue_.pop();

		delete packet;
	}

	while (!recvQueue_.empty())
	{
		Packet* packet = recvQueue_.front();
		recvQueue_.pop();

		delete packet;
	}
}

bool Client::Initialize(int bindPort)
{
	display_ = new LoginDisplay();
	state_ = ClientState::LOGIN;

	if (this->InitSocket() && this->ConnectServer(bindPort))
		return true;
	else
	{
		delete display_;
		return false;
	}
}

bool Client::ConnectToServer()
{
	this->CreateThreads();
	jobThread_ = std::thread(&Client::LogicThread, this);
	return true;
}

/* * * * * * * * * * *
*        Threads
* * * * * * * * * * * */

void Client::SendThread()
{
	while (bIsWorkerRun_)
	{
		Packet* packet = PopQueue();
		if (packet == nullptr)
		{
			Sleep(10); //< 클라이언트의 키 입력이 아무것도 없을 수 있으므로
			continue;
		}

		Stream stream;
		packet->encode(stream);

		int success = send(socket_, (const char*)stream.data(), stream.size(), 0);
		if (success == SOCKET_ERROR) { bIsWorkerRun_ = false; }

		delete packet;
	}
}

void Client::RecvThread()
{
	while (bIsWorkerRun_)
	{
		recvBuf_.fill(0);
		int ioSize = recv(socket_, recvBuf_.data(), recvBuf_.max_size(), 0);
		if (ioSize <= 0)
		{
			printf("[ALERT] 서버와 연결이 끊어졌습니다.\n");
			this->Close();
			break;
		}

		Packet* packet = nullptr;
		packet = PacketFactory::GetPacket(recvBuf_.data(), ioSize);

		if (!packet)
		{
			printf("[ERROR] invalid packet recved\n");
			continue;
		}

		recvQueue_.push(packet);
	}
}

void Client::StateProcessThread()
{
	while (bIsWorkerRun_)
	{
		this->StateProcess();
	}
}

void Client::LogicThread()
{
	while (bIsWorkerRun_)
	{
		if (recvQueue_.empty())
		{
			Sleep(10);
			continue;
		}

		Packet* packet = recvQueue_.front();
		recvQueue_.pop();
		this->RecvProcess(packet);
	}
}


/* * * * * * * * * * * *
*     Queue Manage
* * * * * * * * * * * */


Packet* Client::PopQueue()
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (sendQueue_.empty())
	{
		return nullptr;
	}
	Packet* packet = sendQueue_.front();
	sendQueue_.pop();

	return packet;
}


void Client::PushQueue(Packet* packet)
{
	std::lock_guard<std::mutex> lock(mutex_);
	sendQueue_.push(packet);
}


/* * * * * * * * * * * * * * *
*    Processes
* * * * * * * * * * * * * * */


void Client::RecvProcess(Packet* packet)
{
	PROCESS_FUNCTION fp = processFunc_[packet->type()];
	if (fp != nullptr) (this->*fp)(packet);
	delete packet;
}


void Client::SendNickname()
{
	do {
		display_->draw();
		std::getline(std::cin, nickname_);
	} while (nickname_.empty());

	state_ = ClientState::PENDING;

	PK_C_SERVER_ENTER* packet = new PK_C_SERVER_ENTER();
	packet->nickname = nickname_;

	this->PushQueue(packet);
}


void Client::StateProcess()
{
	if (state_ == ClientState::LOGIN)
	{
		this->SendNickname();
	}
	else if (state_ == ClientState::LOBBY)
	{

	}
}

/* * * * * * * * * * * * * * *
*     Function mapping
* * * * * * * * * * * * * * */



void Client::F_SERVER_ENTER_OK(Packet* packet)
{
	PK_S_SERVER_ENTER_OK* okPacket = static_cast<PK_S_SERVER_ENTER_OK*>(packet);
	delete display_;
	state_ = ClientState::LOBBY;
	userId_ = okPacket->pid;
	/*display_ = new LobbyDisplay(userId_);
	display_->Clear();*/
}
