#ifndef LOGIN_DISPLAY_H
#define LOGIN_DISPLAY_H

#include "Display.h"
#include <iostream>

class LoginDisplay : public Display
{
public:
	void draw()
	{
		this->GetNickname();
	}

private:
	void GetNickname()
	{
		this->Clear();
		printf("����� �г����� �Է��ϼ���!\n�� ");
	}

};

#endif