#pragma once
#include "Res.h"
#include "ByteBuffer.h"

class BufferedStreamProvider: public Resource::IStreamProvider
{
private:
	ByteBuffer buffer;
public:
	BufferedStreamProvider(ByteBuffer& b);

	virtual std::shared_ptr<std::istream> openStream(std::string) override;
};

