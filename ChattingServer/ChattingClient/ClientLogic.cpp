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

