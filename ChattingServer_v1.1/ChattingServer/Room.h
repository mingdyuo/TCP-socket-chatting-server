

#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <set>
#include "Package.h"

class User;
class SendServer;

class Room
{

public:
	Room(uint16_t id, std::string name, SendServer* sendServer) :
		roomId_(id), roomName_(name), sendServer_(sendServer) {}


	void EnterRoom(User* user);
	void ExitRoom(User* user, bool senderExclusive = false);

	void RoomCast(PacketPtr packet);

	uint16_t GetId() const { return roomId_; }
	const std::string& GetName() const { return roomName_; }
	size_t GetUserCount() const { return users_.size(); }


private:
	//< 방 개설자 넣을까 말까

	SendServer* sendServer_;

	std::string			roomName_;
	std::set<User*>		users_;
	const uint16_t		roomId_;

};

#endif // !ROOM_H
