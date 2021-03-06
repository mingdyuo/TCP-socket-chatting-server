#pragma once


#ifndef SERVER_PACKET_H
#define SERVER_PACKET_H

#include "PacketType.h"

class PK_S_SERVER_ENTER_OK : public Packet
{
public:
	PacketType type() { return E_PK_S_SERVER_ENTER_OK; }

	PK_S_SERVER_ENTER_OK() :pid(0) {}
	PK_S_SERVER_ENTER_OK(uint32_t id) :pid(id) {}

	uint32_t pid;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << pid;
	}

	void decode(Stream& stream)
	{
		stream >> &pid;
	}
};

class PK_S_SERVER_ENTER_NO : public Packet
{
public:
	PacketType type() { return E_PK_S_SERVER_ENTER_NO; }
};


class PK_S_LOBBY_ROOM_INFO : public Packet
{
public:
	PacketType type() { return E_PK_S_LOBBY_ROOM_INFO; }

	uint16_t roomCount;
	std::vector<uint16_t> roomId;
	std::vector<std::string> roomName;


	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		roomCount = static_cast<uint16_t> (roomName.size());
		stream << roomCount;
		for (auto id : roomId)
		{
			stream << id;
		}
		for (auto& name : roomName)
		{
			stream << name;
		}
	}

	void decode(Stream& stream)
	{
		stream >> &roomCount;
		std::string name;
		uint16_t id;
		for (int i = 0;i < roomCount;i++)
		{
			stream >> &id;
			roomId.push_back(id);
		}
		for (int i = 0;i < roomCount;i++)
		{
			stream >> &name;
			roomName.push_back(name);
		}
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
		uid(id), nickname(name) {}

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


class PK_S_LOBBY_EXIT : public Packet
{
public:
	PacketType type() { return E_PK_S_LOBBY_EXIT; }
};




class PK_S_ROOM_ENTER : public Packet
{
public:
	PacketType type() { return E_PK_S_ROOM_ENTER; }
	PK_S_ROOM_ENTER() : uid(0) {}
	PK_S_ROOM_ENTER(uint32_t uid, std::string name) :
		uid(uid), name(name){}

	uint32_t uid;
	std::string name;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << uid;
		stream << name;
	}

	void decode(Stream& stream)
	{
		stream >> &uid;
		stream >> &name;
	}
};

class PK_S_ROOM_EXIT : public Packet
{
public:
	PacketType type() { return E_PK_S_ROOM_EXIT; }

	PK_S_ROOM_EXIT() :uid(0){}
	PK_S_ROOM_EXIT(uint32_t uid) :
		uid(uid) {}

	uint32_t uid;


	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << uid;
	}

	void decode(Stream& stream)
	{
		stream >> &uid;
	}
};


class PK_S_ROOM_NAME : public Packet
{
public:
	PacketType type() { return E_PK_S_ROOM_NAME; }
	PK_S_ROOM_NAME() {}
	PK_S_ROOM_NAME(std::string name):
		roomName(name) {}

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

class PK_S_CAST : public Packet
{
public:
	PacketType type() = 0;
	PK_S_CAST() {}
	PK_S_CAST(std::string sender, std::string text) :
		nickname(sender), text(text) {}

	std::string text;
	std::string nickname;

	void encode(Stream& stream)
	{
		stream << (packet_header_size)this->type();
		stream << nickname;
		stream << text;
	}

	void decode(Stream& stream)
	{
		stream >> &nickname;
		stream >> &text;
	}
};

class PK_S_BROADCAST : public PK_S_CAST
{
public:
	PacketType type() { return E_PK_S_BROADCAST; }
	PK_S_BROADCAST() {}
	PK_S_BROADCAST(std::string sender, std::string text)
		: PK_S_CAST(sender, text) {}
};


class PK_S_MULTICAST : public PK_S_CAST
{
public:
	PacketType type() { return E_PK_S_MULTICAST; }
	PK_S_MULTICAST() {}
	PK_S_MULTICAST(std::string sender, std::string text)
		: PK_S_CAST(sender, text) {}
};

class PK_S_UNICAST_OK : public PK_S_CAST
{
public:
	PacketType type() { return E_PK_S_UNICAST_OK; }
	PK_S_UNICAST_OK() {}
	PK_S_UNICAST_OK(std::string sender, std::string text)
		: PK_S_CAST(sender, text) {}
};


class PK_S_UNICAST_NO : public Packet
{
public:
	PacketType type() { return E_PK_S_UNICAST_NO; }
};





#endif // !SERVER_PACKET_H
