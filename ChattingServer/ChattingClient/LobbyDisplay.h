#pragma once

#ifndef LOBBY_DISPLAY_H
#define LOBBY_DISPLAY_H

#include "Display.h"
#include <map>
#include <string>

class LobbyDisplay : public Display
{
public:
	LobbyDisplay(uint32_t pid) :
		userId_(pid)
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


		this->SetColor(CLR_DEFAULT_WHITE);
	}


private:
	std::map<uint32_t, std::string> users_;
	uint32_t userId_;
};


#endif // !LOBBY_DISPLAY_H
