
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
			/* CLIENT */
		case E_PK_C_SERVER_ENTER:
			return new PK_C_SERVER_ENTER();
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