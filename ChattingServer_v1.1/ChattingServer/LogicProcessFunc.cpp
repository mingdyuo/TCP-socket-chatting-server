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
	// uMgr_->SendLobbyInfo(package.session_);
	rMgr_->SendRoomList(package.session_);
	uMgr_->LobbyCast(package.session_, packet->type());

}

void LogicProcess::C_LOBBY_ROOM_INFO(const RecvPackage& package)
{
	rMgr_->SendRoomList(package.session_);
}