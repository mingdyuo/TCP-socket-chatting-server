
#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

using packet_header_size = int32_t;

#include <iostream>
#include <vector>

#include "Stream.h"
#include "ToString.h"


enum PacketType
{
	E_PK_UNDEFINED = 0,

	/* From Client */

	E_PK_C_SERVER_ENTER = 101,
	E_PK_C_SERVER_EXIT = 102,

	E_PK_C_ROOM_ENTER = 201,
	E_PK_C_ROOM_EXIT,

	E_PK_C_BROADCAST,
	E_PK_C_MULTICAST,
	E_PK_C_UNICAST,

	E_PK_C_ROOM_LIST,

	/* From Server */
	E_PK_S_SERVER_ENTER_OK = 501,
	E_PK_S_SERVER_ENTER_NO = 502,

	E_PK_S_LOBBY_USER_INFO,
	E_PK_S_LOBBY_ENTER,
	E_PK_S_LOBBY_EXIT,

	E_PK_S_ROOM_LIST_OK,

	E_PK_S_ROOM_ENTER,
	E_PK_S_ROOM_EXIT,

	E_PK_S_BROADCAST,
	E_PK_S_MULTICAST,
	E_PK_S_UNICAST_OK,
	E_PK_S_UNICAST_NO,

};

class Packet
{
public:
	virtual PacketType type() = 0;
	virtual void encode(Stream& stream) { stream << (packet_header_size)this->type(); }
	virtual void decode(Stream& stream) {}
	virtual void print() { ToString("Packet type : ", this->type()); }
};


class PK_UNDEFINED : public Packet
{
public:
	int type_;

	PK_UNDEFINED() : type_(0) {}
	PK_UNDEFINED(int t) :type_(t) {}
	PacketType type()
	{
		ToString("[WARN] UNDEFINED PACKET USED (", type_, ")\n");
		return E_PK_UNDEFINED;
	}
};

class PK_C_SERVER_ENTER : public Packet
{
public:
	PacketType type() { return E_PK_C_SERVER_ENTER; }

	std::string nickname;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << nickname;
	}

	void decode(Stream& stream)
	{
		stream >> &nickname;
	}

	void print()
	{
		ToString("Nickname :", nickname);
	}
};


class PK_S_SERVER_ENTER_OK : public Packet
{
public:
	PacketType type() { return E_PK_S_SERVER_ENTER_OK; }

	PK_S_SERVER_ENTER_OK() :pid(0){}
	PK_S_SERVER_ENTER_OK(uint32_t id) :pid(id) {}

	uint32_t pid;
	std::string nickname;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << pid;
		stream << nickname;
	}

	void decode(Stream& stream)
	{
		stream >> &pid;
		stream >> &nickname;
	}
};


class PK_S_LOBBY_USER_INFO : public Packet
{
public:
	PacketType type() { return E_PK_S_LOBBY_USER_INFO; }

	PK_S_LOBBY_USER_INFO() : userCount(0), roomCount(0) {}
	PK_S_LOBBY_USER_INFO(uint16_t room) : userCount(0), roomCount(room) {}

	uint16_t roomCount;
	uint16_t userCount;
	std::vector<uint32_t> ids;
	std::vector<std::string> names;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << roomCount;
		userCount = static_cast<uint16_t>(names.size());
		stream << userCount;
		for (auto& id : ids)
		{
			stream << id;
		}
		for (auto& name : names)
		{
			stream << name;
		}
	}

	void decode(Stream& stream)
	{
		stream >> &roomCount;
		stream >> &userCount;
		std::string name;
		uint32_t id;
		for (int i = 0;i < userCount;i++)
		{
			stream >> &id;
			ids.push_back(id);
		}
		for (int i = 0;i < userCount;i++)
		{
			stream >> &name;
			names.push_back(name);
		}
	}
};



class PK_S_LOBBY_ENTER : public Packet
{
public:
	PacketType type() { return E_PK_S_LOBBY_ENTER; }

	PK_S_LOBBY_ENTER() : uid(0) {}
	PK_S_LOBBY_ENTER(uint32_t id, std::string name) : 
		uid(0), nickname(name) {}

	uint32_t uid;
	std::string nickname;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << uid;
		stream << nickname;
	}

	void decode(Stream& stream)
	{
		stream >> &uid;
		stream >> &nickname;
	}
};


#endif