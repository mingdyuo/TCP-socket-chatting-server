#pragma once
#ifndef USER_H
#define USER_H

#include "Session.h"

class Room;

class User
{
public:
	User() = delete;

	User(uint32_t uid) :
		userId_(uid), session_(uid), currRoom_(nullptr)
	{}

public:
	bool Initialize(HANDLE iocpHandle, SOCKET socket)
	{
		return session_.Connect(iocpHandle, socket);
	}

	inline void Release()
	{
		session_.Close();
	}

public:
	void EnterRoom(Room* room)
	{
		if (false == IsInLobby())
		{
			//throw
		}

		currRoom_ = room;
	}

	void ExitRoom()
	{
		if (IsInLobby())
		{
			// throw;
		}

		currRoom_ = nullptr;
	}

public:
	inline void			SetNickname(const std::string nickname) { nickname_ = nickname; }

	inline std::string	GetNickname() const { return nickname_; }

	inline bool			IsInLobby() const { return currRoom_ == nullptr; }

	inline Room*		GetRoom() const { return currRoom_; }

	inline Session*		GetSession() { return &session_; }

	inline uint32_t		GetId() const { return userId_; }


private:
	Session session_;

	uint32_t userId_;
	
	Room* currRoom_;

	std::string nickname_;
};

#endif