#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>
#include <cstdio>

enum DisplayColor
{
	CLR_BLACK = 0,
	CLR_BLUE =1,
	CLR_GREEN =2,
	CLR_CYAN =3,
	CLR_RED =4,
	CLR_MAGENTA =5,
	CLR_BROWN =6,
	CLR_LIGHTGRAY =7,
	CLR_DARKGRAY =8,
	CLR_LIGHTBLUE =9,
	CLR_LIGHTGREEN =10,
	CLR_LIGHTCYAN =11,
	CLR_LIGHTRED =12,
	CLR_LIGHTMAGENTA =13,
	CLR_YELLOW =14,
	CLR_WHITE =15,
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

	inline void SetColor(int TColor, int BColor = CLR_BLACK)
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

