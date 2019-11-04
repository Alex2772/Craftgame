#pragma once
#ifndef SERVER
#include "Gui.h"
#include "Entity.h"
#include "TextureAtlas.h"

class PlayerListEntry: public Gui
{
private:
	void renderSignal(unsigned char count, int x, int y);

public:
	id_type id;
	std::string name;
	short ping;
	char state = 0;
	TextureAtlas icons;
	PlayerListEntry(id_type id, const std::string& name, short ping);

	virtual void render(int flags, int mouseX, int mouseY) override;
};
#endif