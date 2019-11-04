#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <memory.h>
#include "global.h"


#include "CraftgameException.h"
#include <thread>
#include <zconf.h>

class ByteBuffer {
private:
	char* buf = nullptr;
	size_t _size = 0;
	size_t _reserved = 0;
	size_t currentPos = 0;

#ifdef _DEBUG
	std::thread::id thread_id;
#endif
	void reallocate(size_t size);
public:

	ByteBuffer& operator=(const ByteBuffer & rhs);
	ByteBuffer();
	ByteBuffer(const ByteBuffer& b);
	~ByteBuffer();
	template<typename T>
	void put(const T& data) {
		put(reinterpret_cast<const char*>(&data), sizeof(T));
	}
	template<typename T>
	void get(T& data) {
		if (_size - currentPos < sizeof(T)) {
			CraftgameException e(this, "Too short data");
			CrashInfo* ci = new CrashInfo(this);
			ci->addEntry("Read type", std::string(typeid(data).name()) + " (" + std::to_string(sizeof(data)) + ")");
			ci->addEntry("Buffer size", _size);
			ci->addEntry("Data available", _size - currentPos);
			e.addCI(ci);
			throw e;
		}
		get(reinterpret_cast<char*>(&data), sizeof(data));
	}
	void reserve(size_t t);
	void put(const char* c, const size_t size);
	void get(char* c, const size_t size);
	char* get() const;
	char* getCurrentPosAddress() const;
	size_t size() const;
	void seekg(size_t p);
	void setSize(size_t s)
	{
		_size = s;
	}

	size_t reserved() const
	{
		return _reserved;
	}
	size_t count() const {
		return currentPos;
	}

	size_t available() const;
};
template<typename T>
ByteBuffer& operator<<(ByteBuffer& b, const T& data) {
	b.put(data);
	return b;
}
template<typename T>
ByteBuffer& operator>>(ByteBuffer& b, T& data) {
	b.get(data);
	return b;
}
class InventoryContainer;
class ItemStack;
ByteBuffer& operator<<(ByteBuffer& b, InventoryContainer*& c);
ByteBuffer& operator>>(ByteBuffer& b, InventoryContainer*& c);
ByteBuffer& operator<<(ByteBuffer& b, ItemStack*& c);
ByteBuffer& operator>>(ByteBuffer& b, ItemStack*& c);

ByteBuffer& operator<<(ByteBuffer& b, const std::string& data);
ByteBuffer& operator>>(ByteBuffer& b, std::string& data);

ByteBuffer& operator<<(ByteBuffer& b, ByteBuffer& data);
ByteBuffer& operator>>(ByteBuffer& b, ByteBuffer& data);
// Вижуалка тупит
ByteBuffer& operator<<(ByteBuffer& b, const byte& data);
ByteBuffer& operator>>(ByteBuffer& b, byte& data);