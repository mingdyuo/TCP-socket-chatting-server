#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include <map>
#include <string>
#include <set>
#include "Package.h"

class User;
class Room;
class Session;
class SendServer;
class UserManager;


class RoomManager
{

public:
	RoomManager() :
		roomId_(1), sendServer_(nullptr), uMgr_(nullptr) {}

	~RoomManager()
	{
		
	}

	void SetMgr(UserManager* uMgr, SendServer* sServer)
	{
		uMgr_ = uMgr;
		sendServer_ = sServer;
	}

	Room* GetRoom(uint16_t roomId);

	void CreateRoom(uint32_t uid, std::string roomName);

	void BroadCast(PacketPtr packet);

	void SendRoomListToOne(Session* session);

	void SendRoomListToAll();

	void PushIntoLobby(User* user)
	{
		userInLobby_.insert(user);
	}

	void RemoveFromLobby(User* user)
	{
		userInLobby_.erase(user);
	}


private:
	std::map<uint16_t, Room*>	roomList_;
	uint16_t					roomId_;

	std::set<User*>             userInLobby_;

	SendServer* sendServer_;
	UserManager* uMgr_;


};


#endif // !ROOM_MANAGER_H
