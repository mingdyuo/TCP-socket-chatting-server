#include "Client.h"
#include "LoginDisplay.h"

/* * * * * * * * * * * 
*   Initialize & Start
* * * * * * * * * * * */

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

bool Client::ServerEnter()
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
			Sleep(10); //< Ŭ���̾�Ʈ�� Ű �Է��� �ƹ��͵� ���� �� �����Ƿ�
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
			printf("[ALERT] ������ ������ ���������ϴ�.\n");
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
		this->SendProcess();
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
*     Queue Process
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




void Client::RecvProcess(Packet* packet)
{
	if (packet->type() == E_PK_S_SERVER_ENTER_OK)
	{
		// �Լ� �����͸� �� ������ �ڴ�..
	}


	delete packet;
}

