#include "LogicProcess.h"
#include "SendServer.h"
#include "UserManager.h"
#include "RoomManager.h"

void LogicProcess::C_SERVER_ENTER(const RecvPackage& package)
{
	PK_C_SERVER_ENTER* packet = static_cast<PK_C_SERVER_ENTER*>(package.packet_);
	User* user = uMgr_->GetUser(package.session_->GetId());
	user->SetNickname(packet->nickname);

	uMgr_->SendAcceptPacket(package.session_);
	rMgr_->SendRoomListToOne(package.session_);

	//< ���� ���� ���� ������ ��� �߰��� ��쿡 Ȱ��ȭ
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
	

}