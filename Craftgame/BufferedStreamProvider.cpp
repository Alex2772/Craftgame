#include "BufferedStreamProvider.h"
#include "Parsing.h"

BufferedStreamProvider::BufferedStreamProvider(ByteBuffer& b):
	buffer(b)
{
}

std::shared_ptr<std::istream> BufferedStreamProvider::openStream(std::string s)
{
	buffer.seekg(0);
	std::vector<std::string> pathParts = Parsing::splitString(s, "/");
	for (size_t i = 0; i < pathParts.size(); i++)
	{
		byte ct;
		std::string name;
		size_t count;
		buffer >> count;
		for (size_t j = 0; j < count; j++)
		{
			size_t size;
			buffer >> ct;
			buffer >> name;
			buffer >> size;
			switch (ct) {
			case 0x00:
				{
					if (name == pathParts[i])
					{
						std::shared_ptr<std::stringstream> ss(new std::stringstream);
						ss->write(buffer.get() + buffer.count(), size);
						ss->seekg(std::ios_base::beg);
						return ss;
					}
					else
					{
						buffer.seekg(buffer.count() + size);
					}
					break;
				}
			case 0x01:
				if (name == pathParts[i])
				{
					j = count;
				} else
				{
					buffer.seekg(buffer.count() + size);
				}
				break;
			}

		}
	}
	return std::make_shared<std::stringstream>();
}
