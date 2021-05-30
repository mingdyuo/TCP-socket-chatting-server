#pragma once

#ifndef PACKAGE_H
#define PACKAGE_H

#include "../NetworkLib/Packet.h"
#include "Session.h"
#include <memory>


class RecvPackage
{
public:
	Session* session_;
	Packet* packet_;

	RecvPackage()
		:session_(nullptr), packet_(nullptr) {}

	RecvPackage(Session* session, Packet* packet)
		:session_(session), packet_(packet)
	{}

	~RecvPackage()
	{
		session_ = nullptr;
		packet_ = nullptr;
	}

	inline bool Empty()
	{
		return (session_ == nullptr || packet_ == nullptr);
	}
};



typedef std::shared_ptr<Packet> PacketPtr;

class SendPackage
{
public:
	Session* session_;
	PacketPtr packet_;

	SendPackage()
		:session_(nullptr), packet_(nullptr) {}

	SendPackage(Session* session)
		:session_(session), packet_(nullptr) {}

	SendPackage(PacketPtr packet)
		:session_(nullptr), packet_(packet) {}

	SendPackage(Session* session, PacketPtr packet)
		:session_(session), packet_(packet) {}

	~SendPackage()
	{
		session_ = nullptr;
		packet_ = nullptr;
	}

	inline bool Empty()
	{
		return (session_ == nullptr || packet_ == nullptr);
	}
};


#endif