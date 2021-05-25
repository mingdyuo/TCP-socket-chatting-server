

#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <set>
#include "ChatUser.h"

class Room
{

public:
	void EnterRoom();
	void ExitRoom();

	//void MultiCast(Package* packeg)
	//{

	//}



private:

	std::string roomName_;
	std::set<User*> users_;


};

#endif // !ROOM_H
