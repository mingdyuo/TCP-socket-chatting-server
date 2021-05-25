#pragma once

#ifndef LOBBY_DISPLAY_H
#define LOBBY_DISPLAY_H

#include "Display.h"
#include <map>
#include <string>

class LobbyDisplay : public Display
{
public:
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

	void SetRoomList(const std::vector<std::string>& chatrooms)
	{
		chatRooms_ = chatrooms;
	}

	void CursorDown()
	{
		this->EraseCursor();

		int upperLimit = 1 + chatRooms_.size();
		cursorIndex_ = MIN(++cursorIndex_, upperLimit);

		this->DrawCursor();
	}

	void CursorUp()
	{
		this->EraseCursor();

		cursorIndex_= MAX(--cursorIndex_, lowerLimit_);

		this->DrawCursor();
	}

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
		gotoxy(0, 4);
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
	int MIN(int a, int b)
	{
		return a > b ? b : a;
	}
	int MAX(int a, int b)
	{
		return a > b ? a : b;
	}

private:
	short cursorIndex_;
	std::vector<std::string> chatRooms_;
	std::map<uint32_t, std::string> users_;
	uint32_t myId_;
	const int lowerLimit_;
};


#endif // !LOBBY_DISPLAY_H
