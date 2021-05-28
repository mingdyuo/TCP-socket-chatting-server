#include "Client.h"
#include "LoginDisplay.h"
#include "LobbyDisplay.h"



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



/* * * * * * * * * * * * * * *
*     Mapped logic functions
* * * * * * * * * * * * * * */



void Client::F_SERVER_ENTER_OK(Packet* packet)
{
	PK_S_SERVER_ENTER_OK* okPacket = static_cast<PK_S_SERVER_ENTER_OK*>(packet);
	userId_ = okPacket->pid;

	delete display_;
	display_ = new LobbyDisplay(userId_);
	display_->draw();

	state_ = ClientState::LOBBY;

	PK_C_LOBBY_ROOM_INFO* getRoomPacket = new PK_C_LOBBY_ROOM_INFO();
	this->PushQueue(getRoomPacket);
}


void Client::F_LOBBY_ROOM_INFO(Packet* packet)
{
	PK_S_LOBBY_ROOM_INFO* roomPacket = static_cast<PK_S_LOBBY_ROOM_INFO*>(packet);

	LobbyDisplay* display = static_cast<LobbyDisplay*>(display_);
	display->SetRoomList(roomPacket->roomId, roomPacket->roomName);
	display->draw();
}

void Client::F_LOBBY_USER_INFO(Packet* packet)
{
	PK_S_LOBBY_USER_INFO* userPacket = static_cast<PK_S_LOBBY_USER_INFO*>(packet);
}
