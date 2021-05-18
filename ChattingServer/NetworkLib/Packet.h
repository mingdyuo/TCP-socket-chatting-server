
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

	E_PK_S_ROOM_ENTER,
	E_PK_S_ROOM_EXIT,

	E_PK_S_BROADCAST,
	E_PK_S_MULTICAST,
	E_PK_S_UNICAST_OK,
	E_PK_S_UNICAST_NO,

	E_PK_S_ROOM_LIST_OK,
};

class Packet
{
public:
	virtual PacketType type() = 0;
	virtual void encode(Stream& stream) { stream << (packet_header_size)this->type(); }
	virtual void decode(Stream& stream) {}
	virtual void print() { ToString(this->type()); }
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

#endif