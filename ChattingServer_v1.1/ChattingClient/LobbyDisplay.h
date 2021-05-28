#pragma once

#ifndef LOBBY_DISPLAY_H
#define LOBBY_DISPLAY_H

#include "Display.h"
#include <map>
#include <string>

class LobbyDisplay : public Display
{
public:
	LobbyDisplay() = delete;
	LobbyDisplay(uint32_t id) :
		myId_(id), cursorIndex_(4), lowerLimit_(4)
	{}

	void UpdatePlayer(uint32_t id, std::string name)
	{
		users_[id] = name;
	}

	void RemovePlayer(uint32_t pid)
	{
		auto player = users_.find(pid);
		if (player != users_.end())
			users_.erase(pid);
	}

	void draw()
	{
		this->Clear();
		this->DrawWelcome();
		this->DrawList();
		this->DrawCursor();
	}

	void SetRoomList(
		const std::vector<uint16_t>& chatroomId,
		const std::vector<std::string>& chatroomName)
	{
		chatRoomIds_ = chatroomId;
		chatRooms_ = chatroomName;
	}

	void CursorDown()
	{
		this->EraseCursor();

		int upperLimit = lowerLimit_ + chatRooms_.size();
		cursorIndex_ = min(++cursorIndex_, upperLimit);

		this->DrawCursor();
	}

	void CursorUp()
	{
		this->EraseCursor();

		cursorIndex_= max(--cursorIndex_, lowerLimit_);

		this->DrawCursor();
	}

	int GetSelection() const 
	{ return cursorIndex_ == chatRooms_.size() + lowerLimit_ ? 0 : chatRoomIds_[cursorIndex_ - lowerLimit_]; }
		

private:
	
	void DrawWelcome()
	{
		gotoxy(0,0); printf("   / __      ___     // //  ___                       ___      __     //  ___   /  \n");
        gotoxy(0,1); printf("  //   ) ) //___) ) // // //   ) )     //  / /  / / //   ) ) //  ) ) // //   ) /   \n");
        gotoxy(0,2); printf(" //   / / //       // // //   / /     //  / /  / / //   / / //      // //   / /    \n");
        gotoxy(0,3); printf("//   / / ((____   // // ((___/ /     ((__( (__/ / ((___/ / //      // ((___/ /     \n");
	}

	void DrawList() 
	{
		this->SetColor(CLR_WHITE);
		gotoxy(0, lowerLimit_);
		for (int i = 0;i < chatRooms_.size();i++)
		{ 
			printf("[    ] %s\n", chatRooms_[i].c_str());
		}
		printf("[    ] 채팅방 개설하기\n");
	}

	void DrawCursor()
	{
		COORD pos = { 2 , cursorIndex_ };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
		printf("▶");
	}

	void EraseCursor()
	{
		COORD pos = { 2 , cursorIndex_ };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
		printf("  ");
	}

private:
	inline int MIN(int a, int b)
	{
		return a > b ? b : a;
	}
	inline int MAX(int a, int b)
	{
		return a > b ? a : b;
	}

private:
	short cursorIndex_;
	std::vector<std::string> chatRooms_;
	std::vector<uint16_t> chatRoomIds_;
	std::map<uint32_t, std::string> users_;
	uint32_t myId_;
	const int lowerLimit_;
};


#endif // !LOBBY_DISPLAY_H
