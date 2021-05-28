
#ifndef PACKET_FACTORY_H
#define PACKET_FACTORY_H

#include "Packet.h"
#include "Stream.h"


class PacketFactory
{
	using UCHAR = unsigned char;
	using INT64 = signed long long;
public:

	static Packet* ProduceByType(packet_header_size type)
	{
		switch (type)
		{
		case E_PK_UNDEFINED:
			return new PK_UNDEFINED();

			/* * * * * * * * * * *From Client* * * * * * * * * * */
		case E_PK_C_SERVER_ENTER:
			return new PK_C_SERVER_ENTER();
		case E_PK_C_SERVER_EXIT:
			return new PK_C_SERVER_EXIT();
		case E_PK_C_ROOM_ENTER:
			return new PK_C_ROOM_ENTER();
		case E_PK_C_ROOM_EXIT:
			return new PK_C_ROOM_EXIT();

		case E_PK_C_BROADCAST:
			return new PK_C_BROADCAST();
		case E_PK_C_MULTICAST:
			return new PK_C_MULTICAST();
		case E_PK_C_UNICAST:
			return new PK_C_UNICAST();

		case E_PK_C_ROOM_CREATE:
			return new PK_C_ROOM_CREATE();
		case E_PK_C_LOBBY_ROOM_INFO:
			return new PK_C_LOBBY_ROOM_INFO();


			/* * * * * * * * * * *From Server* * * * * * * * * * */
		case E_PK_S_SERVER_ENTER_OK:
			return new PK_S_SERVER_ENTER_OK();
		case E_PK_S_SERVER_ENTER_NO:
			return new PK_S_SERVER_ENTER_NO();

		case E_PK_S_LOBBY_ROOM_INFO:
			return new PK_S_LOBBY_ROOM_INFO();
		case E_PK_S_LOBBY_USER_INFO:
			return new PK_S_LOBBY_USER_INFO();
		case E_PK_S_LOBBY_ENTER:
			return new PK_S_LOBBY_ENTER();
		case E_PK_S_LOBBY_EXIT:
			return new PK_S_LOBBY_EXIT();


		case E_PK_S_ROOM_ENTER:
			return new PK_S_ROOM_ENTER();
		case E_PK_S_ROOM_EXIT:
			return new PK_S_ROOM_EXIT();

		case E_PK_S_BROADCAST:
			return new PK_S_BROADCAST();
		case E_PK_S_MULTICAST:
			return new PK_S_MULTICAST();
		case E_PK_S_UNICAST_OK:
			return new PK_S_UNICAST_OK();
		case E_PK_S_UNICAST_NO:
			return new PK_S_UNICAST_NO();

		default:
			return new PK_UNDEFINED();
		}

	}


	static Packet* GetPacket(const char* rawPacket, size_t size)
	{
		packet_header_size packetType[1] = { E_PK_UNDEFINED, };
		memcpy_s((void*)packetType, sizeof(packetType), (void*)rawPacket, sizeof(packetType));

		Packet* packet = nullptr;
		packet = PacketFactory::ProduceByType(packetType[0]);
		if (packet != nullptr)
		{
			Stream stream((UCHAR*)(rawPacket + sizeof(packetType)), size - sizeof(packetType));
			packet->decode(stream);
		}

		return packet;
	}

	
};

#endif