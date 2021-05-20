#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>
#include <cstdio>


/*
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15
*/

enum DisplayColor
{
	CLR_MY_POSITION = 12,
	CLR_OTHERS_POSITION = 15,
	CLR_MONSTER_POSITION = 5,
	CLR_DEFAULT_WHITE = 15,
	CLR_DEFAULT_BLACK = 0,
	CLR_NPC_TEXT = 1,
	CLR_NPC_BACK = 14,
	CLR_MONSTER = 10

};

class Display
{
public:
	Display()
	{
		CursorView(0);
	}

	virtual ~Display()
	{}

	inline void Clear()
	{
		system("cls");
	}

	virtual void draw() = 0;

	inline void SetColor(int TColor, int BColor = CLR_DEFAULT_BLACK)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), TColor + BColor * 16);
	}

	void CursorView(char show)
	{
		HANDLE hConsole;
		CONSOLE_CURSOR_INFO ConsoleCursor;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		ConsoleCursor.bVisible = show;
		ConsoleCursor.dwSize = 1;

		SetConsoleCursorInfo(hConsole, &ConsoleCursor);
	}

	void gotoxy(short x, short y)
	{
		COORD pos = { x, y };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	}
};



#endif // !DISPLAY_H

