#pragma once


#ifndef CLIENT_PACKET_H
#define CLIENT_PACKET_H

#include "PacketType.h"

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


class PK_C_SERVER_EXIT : public Packet
{
public:
	PacketType type() { return E_PK_C_SERVER_EXIT; }
};

class PK_C_ROOM_ENTER : public Packet
{
public:
	PK_C_ROOM_ENTER() :rid(0){}
	PK_C_ROOM_ENTER(uint16_t rid) :
		rid(rid) {}

	uint16_t rid;

	PacketType type() { return E_PK_C_ROOM_ENTER; }

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << rid;
	}

	void decode(Stream& stream)
	{
		stream >> &rid;
	}
};

class PK_C_ROOM_EXIT : public Packet
{
public:
	PacketType type() { return E_PK_C_ROOM_EXIT; }
};

class PK_C_BROADCAST : public Packet
{
public:
	PacketType type() { return E_PK_C_BROADCAST; }
};

class PK_C_MULTICAST : public Packet
{
public:
	PacketType type() { return E_PK_C_MULTICAST; }
};

class PK_C_UNICAST : public Packet
{
public:
	PacketType type() { return E_PK_C_UNICAST; }
};


class PK_C_ROOM_CREATE : public Packet
{
public:
	PacketType type() { return E_PK_C_ROOM_CREATE; }

	PK_C_ROOM_CREATE() {}
	PK_C_ROOM_CREATE(std::string roomname) :
		roomName(roomname) {}

	std::string roomName;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << roomName;
	}

	void decode(Stream& stream)
	{
		stream >> &roomName;
	}
};

class PK_C_LOBBY_ROOM_INFO : public Packet
{
public:
	PacketType type() { return E_PK_C_LOBBY_ROOM_INFO; }
};


#endif // !CLIENT_PACKET_H
