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

void Room::ExitRoom(User* user, bool senderExclusive)
{
	std::shared_ptr<PK_S_ROOM_EXIT> exitPacket = std::make_shared<PK_S_ROOM_EXIT>(user->GetId());

	if (senderExclusive)
	{
		SendPackage package(exitPacket);
		for (auto& recver : users_)
		{
			if (recver->GetId() == user->GetId())
				continue;
			package.session_ = recver->GetSession();
			sendServer_->PushPackage(package);
		}
	}
	else
	{
		this->RoomCast(exitPacket);
	}

	user->ExitRoom();
	users_.erase(user);
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