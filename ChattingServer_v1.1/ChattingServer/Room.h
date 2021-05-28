

#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <set>
#include "ChatUser.h"

class SendServer;

class Room
{

public:
	Room(uint16_t id, std::string name, SendServer* sendServer) :
		roomId_(id), roomName_(name), sendServer_(sendServer) {}


	void EnterRoom(User* user)
	{
		user->EnterRoom(roomId_);
		users_.insert(user);
	}

	void ExitRoom(User* user)
	{
		user->ExitRoom();
		users_.erase(user);
	}


	
	void RoomCast();


private:
	//< 방 개설자 넣을까 말까

	SendServer* sendServer_;

	std::string			roomName_;
	std::set<User*>		users_;
	const uint16_t		roomId_;

};

#endif // !ROOM_H
