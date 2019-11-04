#include "P06ShowUI.h"
#include "ByteBuffer.h"
#include "GameEngine.h"
#include "GuiScreenMessageDialog.h"

P06ShowUI::P06ShowUI() :
	Packet(0x06)
{
}

P06ShowUI::P06ShowUI(char _ui, const std::string& message)
	: Packet(0x06), message(message)
{
	ui = _ui;
}

void P06ShowUI::write(ByteBuffer& buffer)
{
	buffer << ui;
	buffer << message;
}

void P06ShowUI::read(ByteBuffer& buffer)
{
	buffer >> ui;
	buffer >> message;
}

void P06ShowUI::onReceived()
{
#ifndef SERVER
	char u = ui;
	std::string msg = message;
	CGE::instance->uiThread.push([msg, u]()
	{
		switch (u)
		{
		case 'd':
			CGE::instance->displayGuiScreen(new GuiScreenMessageDialog("", msg));
			break;
		}
	});
#endif
}