#pragma once
#include <string>

namespace OS {
	void setCursorPos(int x, int y);
	std::string locale();
	void getCursorPos(int& x, int& y);
}
