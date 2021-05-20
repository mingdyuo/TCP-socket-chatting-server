#include "Client.h"

void Client::F_SERVER_ENTER_OK(Packet* packet)
{
	PK_S_SERVER_ENTER_OK* okPacket = static_cast<PK_S_SERVER_ENTER_OK*>(packet);
	delete display_;
	state_ = ClientState::LOBBY;
	userId_ = okPacket->pid;
	display_ = new LobbyDisplay(userId_);
	display_->Clear();
}