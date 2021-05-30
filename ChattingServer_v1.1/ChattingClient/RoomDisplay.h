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
		myId_(id), myNickname_(nickname), outputCursor_(6), inputCursor_(7)
	{
		this->CursorView(TRUE);
	}

	void draw()
	{
		this->DrawTopInfo();
		this->SendClear();
	}

	void SetRoomName(std::string name)
	{
		roomName_ = name;
		this->draw();
	}

	void UpdatePlayer(uint32_t uid, const std::string& name)
	{
		users_[uid] = name;
		EnterMessage(name);
	}

	void RemovePlayer(uint32_t uid)
	{
		auto player = users_.find(uid);
		if (player != users_.end())
		{
			this->ExitMessage(users_[uid]);
			users_.erase(uid);
		}
	}

	Packet* SendChat(const std::string& text)
	{
		if (text.find("/all ") == 0 && text.size() > std::string("/all ").size())
		{
			return new PK_C_BROADCAST(text.substr(std::string("/all ").size()));
		}
		else if (text == std::string("/quit"))
		{
			return new PK_C_SERVER_EXIT();
		}
		else if (text == std::string("/lobby"))
		{
			return new PK_C_ROOM_EXIT();
		}
		else if (text.find("/") == 0 && text.find(" ") != std::string::npos)
		{
			return new PK_C_UNICAST(this->ParseForNickname(text), text.substr(text.find(" ") + 1));
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

		if (packet->type() == E_PK_S_UNICAST_NO)
		{
			SetColor(CLR_GREEN);
			printf("[알림] 해당 닉네임의 유저는 존재하지 않습니다.");
		}
		else
		{
			PK_S_CAST* textPacket = static_cast<PK_S_CAST*>(packet);
			switch (packet->type())
			{
			case E_PK_S_MULTICAST:
				SetColor(CLR_WHITE); break;
			case E_PK_S_BROADCAST:
				SetColor(CLR_RED, CLR_YELLOW); break;
			case E_PK_S_UNICAST_OK:
				SetColor(CLR_GREEN); break;
			default:
				break;
			}
			printf("[%s님] %s", textPacket->nickname.c_str(), textPacket->text.c_str());
		}
		SetColor(CLR_WHITE);
		UpdateInputBox();
	}


private:

	void DrawTopInfo()
	{
		gotoxy(0, 0); printf("┏┓╋╋╋╋╋╋┏┓╋╋╋╋╋╋╋╋┏┓╋╋╋╋┏┓╋┏┓　　　　"); printf("[방제목] %s", roomName_.empty() ? "로딩중..": roomName_.c_str());
		gotoxy(0, 1); printf("┃┃╋╋╋╋╋┏┛┗┳┓╋╋╋╋╋╋┃┃╋╋╋┏┛┗┓┃┃　　　　"); printf("현재 인원 (%lld)명", users_.size());
		gotoxy(0, 2); printf("┃┃╋╋╋┏━┻┓┏┫┣━━┓┏━━┫┗━┳━┻┓┏┛┃┃　　　　"); 
		gotoxy(0, 3); printf("┃┃╋╋┏┫┃━┫┃┗┫━━┫┃┏━┫┏┓┃┏┓┃┃╋┗┛　　　　"); printf("전체 채팅 : \"/all\", 귓속말 : \"/nickname\"");
		gotoxy(0, 4); printf("┃┗━━┛┃┃━┫┗┓┣━━┃┃┗━┫┃┃┃┏┓┃┗┓┏┓　　　　"); printf("로비로 돌아가기 : \"/lobby\"");
		gotoxy(0, 5); printf("┗━━━━┻━━┻━┛┗━━┛┗━━┻┛┗┻┛┗┻━┛┗┛　　　　"); printf("종료하기 : \"/quit\"");
	}

	void EnterMessage(const std::string& name)
	{
		gotoxy(0, outputCursor_++);
		printf("[알림] %s님이 입장하셨습니다.", name.c_str());
		this->UpdateInputBox();
	}

	void ExitMessage(const std::string& name)
	{
		gotoxy(0, outputCursor_++);
		printf("[알림] %s님이 퇴장하셨습니다.", name.c_str());
		this->UpdateInputBox();
	}

	std::string ParseForNickname(std::string text)
	{
		return text.substr(1, text.find_first_of(" ") - 1);;
	}

	void UpdateInputBox()
	{
		gotoxy(0, inputCursor_++); printf("%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27, 27, 27);
		gotoxy(0, inputCursor_); printf("〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓\n[%s님]: ", myNickname_.c_str());
	}

	void SendClear()
	{
		gotoxy(0, inputCursor_); printf("%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27, 27, 27);
		gotoxy(0, inputCursor_); printf("〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓\n[%s님]: ", myNickname_.c_str());

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
