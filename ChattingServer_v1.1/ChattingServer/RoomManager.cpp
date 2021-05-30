#include "RoomManager.h"
#include "UserManager.h"
#include "SendServer.h"
#include "Room.h"

Room* RoomManager::GetRoom(uint16_t roomId)
{
	auto room = roomList_.find(roomId);
	if (room == roomList_.end())
	{
		return nullptr;
	}
	return room->second;
}

void RoomManager::CreateRoom(uint32_t uid, std::string roomName)
{
	Room* newRoom = new Room(roomId_, roomName, sendServer_);
	roomList_.insert({ roomId_++, newRoom });

	this->SendRoomListToOne(uMgr_->GetSession(uid));
}

void RoomManager::BroadCast(PacketPtr packet)
{
	for (auto& room : roomList_)
	{
		room.second->RoomCast(packet);
		// Multicast in each room
	}
}


void RoomManager::SendRoomListToOne(Session* session)
{

	std::shared_ptr<PK_S_LOBBY_ROOM_INFO> roomPacket = std::make_shared<PK_S_LOBBY_ROOM_INFO>();
	roomPacket->roomCount = roomList_.size();
	for (auto& room : roomList_)
	{
		roomPacket->roomId.push_back(room.second->GetId());
		roomPacket->roomName.push_back(room.second->GetName());
	}
	SendPackage package(session, roomPacket);
	sendServer_->PushPackage(package);
	
	// TODO : 채팅방 정보 보내기

					//< 후순위 구현 : 현재 로비에 접속중인 유저
	//std::shared_ptr<PK_S_LOBBY_USER_INFO> userPacket = std::make_shared<PK_S_LOBBY_USER_INFO>(userList_.size());
	//for (auto& user : userList_)
	//{
	//	userPacket->ids.push_back(user.second->GetId());
	//	userPacket->names.push_back(user.second->GetNickname());
	//}

	//SendPackage package(session, userPacket);
	//sendServer_->PushPackage(package);
}


void RoomManager::SendRoomListToAll()
{
	std::shared_ptr<PK_S_LOBBY_ROOM_INFO> roomPacket = std::make_shared<PK_S_LOBBY_ROOM_INFO>();
	roomPacket->roomCount = roomList_.size();
	for (auto& room : roomList_)
	{
		roomPacket->roomId.push_back(room.second->GetId());
		roomPacket->roomName.push_back(room.second->GetName());
	}

	SendPackage package(nullptr, roomPacket);
	for (auto& user : userInLobby_)
	{
		package.session_ = user->GetSession();
		sendServer_->PushPackage(package);
	}
}