#include "Client.h"
#include "LoginDisplay.h"
#include "LobbyDisplay.h"


/* * * * * * * * * * * * * * *
*     Mapped logic functions
* * * * * * * * * * * * * * */



void Client::F_SERVER_ENTER_OK(Packet* packet)
{
	PK_S_SERVER_ENTER_OK* okPacket = static_cast<PK_S_SERVER_ENTER_OK*>(packet);
	delete display_;
	state_ = ClientState::LOBBY;
	userId_ = okPacket->pid;

	display_ = new LobbyDisplay(userId_);
	display_->draw();
}


void Client::F_LOBBY_ROOM_INFO(Packet* packet)
{
	PK_S_LOBBY_ROOM_INFO* roomPacket = static_cast<PK_S_LOBBY_ROOM_INFO*>(packet);
}

void Client::F_LOBBY_USER_INFO(Packet* packet)
{
	PK_S_LOBBY_USER_INFO* userPacket = static_cast<PK_S_LOBBY_USER_INFO*>(packet);
}
