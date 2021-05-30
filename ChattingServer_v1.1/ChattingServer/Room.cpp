#include "Room.h"
#include "ChatUser.h"
#include "SendServer.h"

void Room::EnterRoom(User* user)
{
	user->EnterRoom(roomId_);
	users_.insert(user);

	std::shared_ptr<PK_S_ROOM_ENTER> enterPacket = std::make_shared<PK_S_ROOM_ENTER>(user->GetId(), this->roomId_);
	this->RoomCast(enterPacket);
}

void Room::ExitRoom(User* user)
{
	user->ExitRoom();
	users_.erase(user);

	std::shared_ptr<PK_S_ROOM_EXIT> exitPacket = std::make_shared<PK_S_ROOM_EXIT>(user->GetId(), this->roomId_);
	this->RoomCast(exitPacket);
}



void Room::RoomCast(PacketPtr packet)
{
	SendPackage package(packet);
	for (auto& user : users_)
	{
		package.session_ = user->GetSession();
		sendServer_->PushPackage(package);
	}
}