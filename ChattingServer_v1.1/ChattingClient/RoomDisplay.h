#pragma once

#ifndef ROOM_DISPLAY_H
#define ROOM_DISPLAY_H

#include "Display.h"
#include "../NetworkLib/PacketFactory.h"
#include <map>
#include <string>

class RoomDisplay : public Display
{
public:
	RoomDisplay() = delete;
	RoomDisplay(uint32_t id, std::string nickname) :
		myId_(id), myNickname_(nickname), outputCursor_(6), inputCursor_(6)
	{
		this->CursorView(TRUE);
	}

	void draw()
	{
		this->Clear();
		this->DrawTopInfo();
		this->SetCursorToInputBox();
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

	void OnRecv()
	{

	}

	Packet* SendChat(std::string text)
	{
		// 어떤 방식 캐스트인지 확인

		return nullptr;
	}

	void OnSend()
	{
		// input 인덱스 올리기 
	}

private:

	void DrawTopInfo()
	{
		gotoxy(0, 0); printf("┏┓╋╋╋╋╋╋┏┓╋╋╋╋╋╋╋╋┏┓╋╋╋╋┏┓╋┏┓　　　　"); printf("[방제목] %s", roomName_.empty() ? "로딩중..": roomName_.c_str());
		gotoxy(0, 1); printf("┃┃╋╋╋╋╋┏┛┗┳┓╋╋╋╋╋╋┃┃╋╋╋┏┛┗┓┃┃　　　　"); printf("현재 인원 (%lld)명", users_.size());
		gotoxy(0, 2); printf("┃┃╋╋╋┏━┻┓┏┫┣━━┓┏━━┫┗━┳━┻┓┏┛┃┃　　　　"); 
		gotoxy(0, 3); printf("┃┃╋╋┏┫┃━┫┃┗┫━━┫┃┏━┫┏┓┃┏┓┃┃╋┗┛　　　　");
		gotoxy(0, 4); printf("┃┗━━┛┃┃━┫┗┓┣━━┃┃┗━┫┃┃┃┏┓┃┗┓┏┓　　　　");
		gotoxy(0, 5); printf("┗━━━━┻━━┻━┛┗━━┛┗━━┻┛┗┻┛┗┻━┛┗┛　　　　");
	}

	void EnterMessage(const std::string& name)
	{
		printf("[알림] %s님이 입장하셨습니다.", name.c_str());
	}

	void ExitMessage(const std::string& name)
	{
		printf("[알림] %s님이 퇴장하셨습니다.", name.c_str());
	}

	void SetCursorToInputBox()
	{
		gotoxy(0, inputCursor_); printf("%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K\n%c[2K", 27, 27, 27, 27, 27, 27);
		gotoxy(0, inputCursor_); printf("〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓〓\n[%s님]: ", myNickname_.c_str());
	}

private:
	short inputCursor_;
	short outputCursor_;

	std::map<uint32_t, std::string> users_;
	uint32_t myId_;
	std::string myNickname_;
	std::string roomName_;
};


#endif // !ROOM_DISPLAY_H
