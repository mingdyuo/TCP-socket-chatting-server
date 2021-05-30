#pragma once

#ifndef ROOM_DISPLAY_H
#define ROOM_DISPLAY_H

#include "Display.h"
#include <map>
#include <string>

class RoomDisplay : public Display
{
public:
	RoomDisplay() = delete;
	RoomDisplay(uint32_t id) :
		myId_(id), outputCursor_(0), inputCursor_(0)
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
		this->DrawTopInfo();
		this->DrawCursor();
	}



private:

	void DrawTopInfo()
	{


	}



	void DrawCursor()
	{

	}

	void EraseCursor()
	{
		COORD pos = { 2 , outputCursor_ };
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
	short inputCursor_;
	short outputCursor_;

	std::map<uint32_t, std::string> users_;
	uint32_t myId_;
};


#endif // !ROOM_DISPLAY_H
