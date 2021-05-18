#include "Client.h"
#include "LoginDisplay.h"

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


void Client::RecvProcess(Packet* packet)
{
	if (packet->type() == E_PK_S_SERVER_ENTER_OK)
	{
		// 함수 포인터를 걍 만들어야 겠다..
	}
	

	delete packet;
}

