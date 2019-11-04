#include "def.h"
Pos::Pos(DPos& p)
{
	x = round(p.x);
	y = round(p.y);
	z = round(p.z);
}
Pos operator+(const Pos& l, const Pos& r) {
	Pos res;
	res.x = l.x + r.x;
	res.y = l.y + r.y;
	res.z = l.z + r.z;
	return res;
}
Pos operator-(const Pos& l, const Pos& r) {
	Pos res;
	res.x = l.x - r.x;
	res.y = l.y - r.y;
	res.z = l.z - r.z;
	return res;
}
DPos operator+(const DPos& l, const DPos& r) {
	DPos res;
	res.x = l.x + r.x;
	res.y = l.y + r.y;
	res.z = l.z + r.z;
	return res;
}
DPos operator-(const DPos& l, const DPos& r) {
	DPos res;
	res.x = l.x - r.x;
	res.y = l.y - r.y;
	res.z = l.z - r.z;
	return res;
}

bool operator==(const glm::vec3& l, const DPos& r)
{
	return l.x == r.x && l.y == r.y && l.z == r.z;
}
bool operator!=(const glm::vec3& pos, const DPos& rhs)
{
	return !(pos == rhs);
}

bool operator==(const DPos& l, const DPos& r)
{
	return l.x == r.x && l.y == r.y && l.z == r.z;
}

bool operator!=(const DPos& l, const DPos& r)
{
	return !(l == r);
}

bool operator==(const Pos& x, const Pos& y) {
	return x.x == y.x &&
		x.y == y.y &&
		x.z == y.z;
}