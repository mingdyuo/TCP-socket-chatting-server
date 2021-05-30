#include "LogicProcess.h"
#include "SendServer.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "Room.h"

void LogicProcess::C_SERVER_ENTER(const RecvPackage& package)
{
	PK_C_SERVER_ENTER* packet = static_cast<PK_C_SERVER_ENTER*>(package.packet_);
	User* user = uMgr_->GetUser(package.session_->GetId());
	user->SetNickname(packet->nickname);

	uMgr_->SendAcceptPacket(package.session_);
	rMgr_->SendRoomListToOne(package.session_);

	//< 추후 유저 정보 보내는 기능 추가할 경우에 활성화
	// rMgr_->SendRoomListToAll();
}

void LogicProcess::C_LOBBY_ROOM_INFO(const RecvPackage& package)
{
	rMgr_->SendRoomListToOne(package.session_);
}


void LogicProcess::C_ROOM_CREATE(const RecvPackage& package)
{
	PK_C_ROOM_CREATE* packet = static_cast<PK_C_ROOM_CREATE*>(package.packet_);
	rMgr_->CreateRoom(package.session_->GetId(), packet->roomName);
}

void LogicProcess::C_ROOM_ENTER(const RecvPackage& package)
{
	PK_C_ROOM_ENTER* packet = static_cast<PK_C_ROOM_ENTER*>(package.packet_);
	Room* room = rMgr_->GetRoom(packet->rid);
	room->EnterRoom(uMgr_->GetUser(package.session_->GetId()));

}

void LogicProcess::C_UNICAST(const RecvPackage& package)
{

}

void LogicProcess::C_MULTICAST(const RecvPackage& package)
{
	User* user = uMgr_->GetUser(package.session_->GetId());
	Room* room = user->GetRoom();
	
	PK_C_MULTICAST* recvPacket = static_cast<PK_C_MULTICAST*>(package.packet_);
	std::shared_ptr<PK_S_MULTICAST> packet = std::make_shared<PK_S_MULTICAST>(user->GetNickname(), recvPacket->text);
	room->RoomCast(packet);
}

void LogicProcess::C_BROADCAST(const RecvPackage& package)
{

}
