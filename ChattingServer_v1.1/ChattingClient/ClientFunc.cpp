#include "Client.h"
#include "LoginDisplay.h"
#include "LobbyDisplay.h"
#include "RoomDisplay.h"



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
*     Mapped logic functions as received packet from server
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

	if (roomPacket->roomCount == 0)
		return;

	LobbyDisplay* display = static_cast<LobbyDisplay*>(display_);
	display->SetRoomList(roomPacket->roomId, roomPacket->roomName);
	display->draw();
}

void Client::F_LOBBY_USER_INFO(Packet* packet)
{
	PK_S_LOBBY_USER_INFO* userPacket = static_cast<PK_S_LOBBY_USER_INFO*>(packet);
}


void Client::F_ROOM_ENTER(Packet* packet)
{
	PK_S_ROOM_ENTER* enterPacket = static_cast<PK_S_ROOM_ENTER*>(packet);
	if (state_ == ClientState::LOBBY)
	{
		delete display_;
		display_ = new RoomDisplay(this->userId_, this->nickname_);
		display_->Clear();
		display_->draw();
		state_ = ClientState::CHATROOM;
	}
	
	RoomDisplay* display = static_cast<RoomDisplay*>(display_);
	display->UpdatePlayer(enterPacket->uid, enterPacket->name);

}


void Client::F_ROOM_EXIT(Packet* packet)
{
	PK_S_ROOM_EXIT* exitPacket = static_cast<PK_S_ROOM_EXIT*>(packet);
	if (exitPacket->uid == this->userId_)
	{
		// ���� ���� ��
		delete display_;
		display_ = new LobbyDisplay(this->userId_);
		state_ = ClientState::LOBBY;
		display_->draw();
	}
	else
	{
		// �ٸ� ����� ���� ��
		RoomDisplay* display = static_cast<RoomDisplay*>(display_);
		display->RemovePlayer(exitPacket->uid);
	}
}

void Client::F_ROOM_NAME(Packet* packet)
{
	PK_S_ROOM_NAME* namepacket = static_cast<PK_S_ROOM_NAME*>(packet);
	RoomDisplay* display = static_cast<RoomDisplay*>(display_);
	display->SetRoomName(namepacket->roomName);
}

void Client::F_RECV_CAST(Packet* packet)
{
	RoomDisplay* display = static_cast<RoomDisplay*>(display_);
	display->OnRecv(packet);
}