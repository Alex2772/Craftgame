#pragma once
#include "IDrawable.h"
#ifndef SERVER
#include <vector>
#include <string>
#include "GuiScreenDialog.h"
#include "CFont.h"
#include "LockableData.h"
#include <boost/python.hpp>

#define MD_OK 1
#define MD_CANCEL 2

class GuiScreenMessageDialog : public GuiScreenDialog {
protected:
	std::string orig;
	std::vector<std::string> text;
	CFont* f;
	std::function<void(int)> proc;
public:
	GuiScreenMessageDialog(std::string title, std::string text, std::function<void(int)> proc, uint8_t t = MD_OK);
	GuiScreenMessageDialog(std::string title, std::string text, boost::python::object, uint8_t t = MD_OK);
	GuiScreenMessageDialog(std::string title, std::string text);
	virtual ~GuiScreenMessageDialog()
	{
	}
	void setText(std::string s);
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void close();
	virtual void onLayoutChange(int x, int y, int width, int height);
};

#endif