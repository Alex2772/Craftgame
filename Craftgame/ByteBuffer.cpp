#include "ByteBuffer.h"

ByteBuffer::ByteBuffer() {

}

void ByteBuffer::put(const char* c, const size_t size) {
	buf.insert(buf.end(), c, c + size);
}
void ByteBuffer::get(char* c, const size_t size) {
	memcpy(c, buf.data() + currentPos, size);
	currentPos += size;
}
const char* ByteBuffer::get() {
	return buf.data();
}
size_t ByteBuffer::size() {
	return buf.size();
}

ByteBuffer& operator<<(ByteBuffer& b, std::string& data) {
	size_t s = data.size();
	b.put(s);
	if (s)
		b.put(data.c_str(), data.size());
	return b;
}
ByteBuffer& operator >> (ByteBuffer& b, std::string& data) {
	size_t t;
	b.get(t);
	if (t) {
		char* d = new char[t + 1];
		d[t] = '\0';
		b.get(d, t);
		data = std::string(d);
		delete[] d;
	}
	return b;
}