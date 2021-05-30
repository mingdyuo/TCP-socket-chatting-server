#pragma once

#ifndef ROOM_DISPLAY_H
#define ROOM_DISPLAY_H

#include "Display.h"
#include "../NetworkLib/PacketFactory.h"
#include <map>
#include <string>
#include <mutex>



class RoomDisplay : public Display
{
public:
	RoomDisplay() = delete;
	RoomDisplay(uint32_t id, std::string nickname) :
		myId_(id), myNickname_(nickname), outputCursor_(6), inputCursor_(10)
	{
		this->CursorView(TRUE);
	}

	void draw()
	{
		this->Clear();
		this->DrawTopInfo();
		this->SendClear();
	}

	void SetRoomName(std::string name)
	{
		roomName_ = name;
		this->draw();
	}

	void UpdatePlayer(uint32_t id, const std::string& name)
	{
		users_[id] = name;
		EnterMessage(name);
	}

	void RemovePlayer(uint32_t pid)
	{
		auto player = users_.find(pid);
		if (player != users_.end())
		{
			this->ExitMessage(users_[pid]);
			users_.erase(pid);
		}
	}

	Packet* SendChat(const std::string& text)
	{
		if (text.find("/all ") == 0 && text.size() > std::string("/all ").size())
		{
			return new PK_C_BROADCAST(text.substr(std::string("/all ").size()));
		}
		else if (text.find("/") == 0 && text.find(" ") != std::string::npos)
		{
			return new PK_C_UNICAST(this->ParseForNickname(text), text);
		}

		return new PK_C_MULTICAST(text);
	}

	void OnSend()
	{
		this->SendClear();
	}

	void OnRecv(Packet* packet)
	{
		gotoxy(0, outputCursor_++);

		switch (packet->type())
		{
		case E_PK_S_MULTICAST:
		{
			PK_S_MULTICAST* textPacket = static_cast<PK_S_MULTICAST*>(packet);
			SetColor(CLR_WHITE);
			printf("[%s��] %s", textPacket->nickname.c_str(), textPacket->text.c_str());
			break;
		}
		case E_PK_S_BROADCAST:
		{
			SetColor(CLR_WHITE, CLR_YELLOW);
			break;
		}
		case E_PK_S_UNICAST_OK:
		{
			SetColor(CLR_GREEN);
			break;
		}
		case E_PK_S_UNICAST_NO:
		{
			SetColor(CLR_GREEN);
			printf("[�˸�] �ش� �г����� ������ �������� �ʽ��ϴ�.");
			break;
		}
		default:
			break;
		}

		UpdateInputBox();
	}


private:

	void DrawTopInfo()
	{
		gotoxy(0, 0); printf("������������������������������������������������������������������"); printf("[������] %s", roomName_.empty() ? "�ε���..": roomName_.c_str());
		gotoxy(0, 1); printf("������������������������������������������������������������������"); printf("���� �ο� (%lld)��", users_.size());
		gotoxy(0, 2); printf("������������������������������������������������������������������"); 
		gotoxy(0, 3); printf("������������������������������������������������������������������"); printf("��ü ä�� : \"/all ä�ó���\"");
		gotoxy(0, 4); printf("������������������������������������������������������������������"); printf("�ӼӸ� : \"/nickname ä�ó���\"");
		gotoxy(0, 5); printf("������������������������������������������������������������������");
	}

	void EnterMessage(const std::string& name)
	{
		printf("[�˸�] %s���� �����ϼ̽��ϴ�.", name.c_str());
	}

	void ExitMessage(const std::string& name)
	{
		printf("[�˸�] %s���� �����ϼ̽��ϴ�.", name.c_str());
	}

	std::string ParseForNickname(std::string text)
	{
		return text.substr(1, text.find_first_of(" "));;
	}

	void UpdateInputBox()
	{
		gotoxy(0, inputCursor_++); printf("%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27, 27, 27);
		gotoxy(0, inputCursor_); printf("��������������������������\n[%s��]: ", myNickname_.c_str());
	}

	void SendClear()
	{
		gotoxy(0, inputCursor_); printf("%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27, 27, 27);
		gotoxy(0, inputCursor_); printf("��������������������������\n[%s��]: ", myNickname_.c_str());

	}


	

private:
	static const std::string BROADCAST_COMMAND;

private:
	short inputCursor_;
	short outputCursor_;

	std::string						roomName_;
	std::map<uint32_t, std::string>	users_;

	uint32_t						myId_;
	std::string						myNickname_;

	std::mutex						mutex_;
};


#endif // !ROOM_DISPLAY_H
