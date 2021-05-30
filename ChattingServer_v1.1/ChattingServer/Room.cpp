#include "Room.h"
#include "ChatUser.h"
#include "SendServer.h"

void Room::EnterRoom(User* user)
{
	user->EnterRoom(this);
	users_.insert(user);

	std::shared_ptr<PK_S_ROOM_ENTER> enterPacket = std::make_shared<PK_S_ROOM_ENTER>(user->GetId(), user->GetNickname());
	this->RoomCast(enterPacket);

	std::shared_ptr<PK_S_ROOM_NAME> namePacket = std::make_shared<PK_S_ROOM_NAME>(this->roomName_);
	SendPackage package(user->GetSession(), namePacket);
	sendServer_->PushPackage(package);
}

void Room::ExitRoom(User* user)
{
	user->ExitRoom();
	users_.erase(user);

	std::shared_ptr<PK_S_ROOM_EXIT> exitPacket = std::make_shared<PK_S_ROOM_EXIT>(user->GetId());
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