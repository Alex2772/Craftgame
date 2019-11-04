#pragma once
#include "Packet.h"
#include "ByteBuffer.h"

class PAutocomplete : public Packet {
public:
	std::string cmd;
	size_t index;
	std::vector<std::string> autocomplete;
	char mode;
	PAutocomplete(std::string c, size_t i):
		Packet(0x10),
		cmd(c),
		index(i),
		mode(0)
	{
		
	}
	PAutocomplete() :
		Packet(0x10)
	{

	}
	virtual void write(ByteBuffer& buffer)
	{
		buffer << mode;
		switch(mode)
		{
		case 0:
			buffer << cmd;
			buffer << index;
			break;
		case 1:
			size_t c = autocomplete.size();
			buffer << c;
			for (std::string& s : autocomplete)
				buffer << s;
			break;
		}
	}
	virtual void read(ByteBuffer& buffer)
	{
		buffer >> mode;
		switch (mode)
		{
		case 0:
			buffer >> cmd;
			buffer >> index;
			break;
		case 1:
			size_t cnt;
			buffer >> cnt;
			autocomplete.reserve(cnt);
			for (size_t i = 0; i < cnt; i++)
			{
				std::string s;
				buffer >> s;
				autocomplete.push_back(s);
			}
			break;
		}
	}
	virtual void onReceived();
	virtual void onReceived(EntityPlayerMP*);
};
