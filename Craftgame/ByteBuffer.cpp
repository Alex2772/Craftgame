#include "ByteBuffer.h"
#include <algorithm>
#include <cassert>
#include "Slot.h"
#include "InventoryContainer.h"
#include "GameEngine.h"

ByteBuffer::ByteBuffer() {
#ifdef _DEBUG
	thread_id = std::this_thread::get_id();
#endif
}

ByteBuffer::ByteBuffer(const ByteBuffer& b)
{
	_size = b._size;
	reallocate(_size);
	memcpy(buf, b.buf, _size);
	currentPos = b.currentPos;
}

ByteBuffer::~ByteBuffer() {
	//assert(thread_id == std::this_thread::get_id());
	if (buf)
		delete[] buf;
}
void ByteBuffer::reallocate(size_t size)
{
	//assert(thread_id == std::this_thread::get_id());
	char* nBuffer = new char[size];
	_reserved = size;
	if (buf) {
		memcpy(nBuffer, buf, min(_size, size));
		delete[] buf;
	}
	buf = nBuffer;
}

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& b)
{
	_size = b._size;
	reallocate(_size);
	memcpy(buf, b.buf, _size);
	currentPos = b.currentPos;
	return *this;
}

/**
 * \brief Расширяет буфер на t байт. Эта функция необязательна, но она ускоряет работу вектора при его заполнении
 */
void ByteBuffer::reserve(size_t t)
{
	//assert(thread_id == std::this_thread::get_id());
	reallocate(_size + t);
}

void ByteBuffer::put(const char* c, const size_t size) {
	//assert(thread_id == std::this_thread::get_id());
	if (currentPos + size > _reserved)
	{
		reallocate(currentPos + size + 128);
	}
	memcpy(buf + currentPos, c, size);
	currentPos += size;
	_size += size;
}
void ByteBuffer::get(char* c, const size_t size) {
	//assert(thread_id == std::this_thread::get_id());
	assert(_size >= currentPos + size);
	memcpy(c, buf + currentPos, size);
	currentPos += size;
}
char* ByteBuffer::get() const
{
	//assert(thread_id == std::this_thread::get_id());
	return buf;
}

/**
 * \return Адрес текущей позиции
 */
char* ByteBuffer::getCurrentPosAddress() const
{
	return buf + currentPos;
}

size_t ByteBuffer::size() const
{
	//assert(thread_id == std::this_thread::get_id());
	return _size;
}

void ByteBuffer::seekg(size_t p)
{
	//assert(thread_id == std::this_thread::get_id());
	currentPos = p;
}

/**
 * \return Количество доступных байт
 */
size_t ByteBuffer::available() const
{
	return _size - currentPos;
}

ByteBuffer & operator<<(ByteBuffer & b, InventoryContainer *& c)
{
	for (Slot*& s: c->slots)
	{
		b << s->stack;
	}
	return b;
}

ByteBuffer& operator>>(ByteBuffer& b, InventoryContainer*& c)
{
	for (Slot*& s : c->slots)
	{
		if (s->stack)
			delete s->stack;
		b >> s->stack;
		if (s->stack) {
			INFO(std::string("Stack " + s->stack->getName()));
			if (boost::shared_ptr<ADT::Map> n = s->stack->getADT()) {
				char buf[64];
				sprintf(buf, "%p", n.get());
				INFO(std::string("    ADT: [") + std::to_string(n->id) + "]: " + buf);
			}
		}
	}
	return b;
}

ByteBuffer & operator<<(ByteBuffer & b, ItemStack *& c)
{
	if (c) {
		b << c->getCount();
		b << c->getItem()->getId();
		b << c->getItem()->getData();
		if (c->getRawADT()) {
			b << uint8_t(1);
			ADT::serialize(b, c->getRawADT());
		} else
		{
			b << uint8_t(0);
		}
	} else
	{
		b << uint16_t(0);
	}
	return b;
}

ByteBuffer & operator>>(ByteBuffer & b, ItemStack *& c)
{
	bid id;
	byte data;
	uint16_t count;
	b >> count;
	if (count) {
		b >> id >> data;
		Item* item = CGE::instance->goRegistry->getGO(id, data);
		if (item) {
			c = item->newStack();
			c->setCount(count);
		}
		uint8_t k;
		b >> k;
		if (item && k)
		{
			boost::shared_ptr<ADT::Map> adt = ADT::deserialize(b)->toMap();
			c->setADT(adt);
		}
	} else
	{
		c = nullptr;
	}
	return b;
}

ByteBuffer& operator<<(ByteBuffer& b, const std::string& data) {
	size_t s = data.size();
	b << s;
	if (s)
		b.put(data.c_str(), data.size());
	return b;
}
ByteBuffer& operator >> (ByteBuffer& b, std::string& data) {
	size_t t;
	b >> t;
	if (t) {
		char* d = new char[t + 1];
		memset(d, 0, t + 1);
		b.get(d, t);
		data = d;
		delete[] d;
	}
	return b;
}
ByteBuffer& operator<<(ByteBuffer& b, const byte& data) {
	b.put(data);
	return b;
}
ByteBuffer& operator>>(ByteBuffer& b, byte& data) {
	b.get(data);
	return b;
}

ByteBuffer& operator<<(ByteBuffer& b, ByteBuffer& data) {
	b << data.size();
	if (data.size())
	{
		b.put(data.get(), data.size());
	}
	return b;
}
ByteBuffer& operator>>(ByteBuffer& b, ByteBuffer& data) {
	size_t size;
	b >> size;
	if (size && size < 1024 * 1024 * 1024)
	{
		char* buffer = new char[size];
		b.get(buffer, size);
		data.put(buffer, size);
		delete[] buffer;
	}
	return b;
}