#include "RoomManager.h"
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

void RoomManager::CreateRoom(std::string roomName)
{

}

void RoomManager::MultiCast(uint16_t roomId)
{

}

void RoomManager::BroadCast()
{
	for (auto& room : roomList_)
	{
		// Multicast in each room
	}
}


void RoomManager::SendRoomList(Session* session)
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