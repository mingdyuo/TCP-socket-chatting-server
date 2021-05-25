#pragma once
#ifndef USER_H
#define USER_H

#include "Session.h"

class User
{
public:
	User() = delete;

	User(uint32_t uid) :
		userId_(uid), session_(uid), currRoom_(0)
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
	bool EnterRoom(uint16_t no)
	{
		if (false == IsInRoom())
			return false;

		currRoom_ = no;
		return true;
	}

	bool ExitRoom()
	{
		if (IsInRoom())
			return false;

		currRoom_ = 0;
		return true;
	}

public:
	inline void			SetNickname(const std::string nickname) { nickname_ = nickname; }

	inline std::string	GetNickname() const { return nickname_; }

	inline bool			IsInRoom() const { return currRoom_ == 0; }

	inline uint16_t		GetRoom() const { return currRoom_; }

	inline Session*		GetSession() { return &session_; }

	inline uint32_t		GetId() const { return userId_; }


private:
	Session session_;

	uint32_t userId_;
	uint16_t currRoom_;

	std::string nickname_;
};

#endif