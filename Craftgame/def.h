#pragma once
#include <cmath>
#include <glm/detail/type_vec3.hpp>
#include <glm/glm.hpp>

typedef int cindex;
typedef int p_type;
typedef double dtype;
typedef unsigned short bid;
typedef unsigned short world_time;

class DPos;

class Pos
{
public:
	p_type x;
	p_type y;
	p_type z;
	Pos(p_type _x, p_type _y, p_type _z):
		x(_x),
		y(_y),
		z(_z)
	{
		
	}
	Pos(DPos& p);
	Pos():
		x(0),
		y(0),
		z(0)
	{
		
	}
};

class DPos
{
public:
	DPos(const Pos& p)
	{
		x = dtype(p.x);
		y = dtype(p.y);
		z = dtype(p.z);
	}
	DPos(dtype _x, dtype _y, dtype _z) :
		x(_x),
		y(_y),
		z(_z)
	{

	}
	DPos() :
		x(0),
		y(0),
		z(0)
	{

	}
	DPos(glm::vec3 p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
	}
	DPos& operator+=(const DPos& c) {
		x += c.x;
		y += c.y;
		z += c.z;
		return *this;
	}
	DPos& operator-=(const DPos& c) {
		x -= c.x;
		y -= c.y;
		z -= c.z;
		return *this;
	}

	::DPos& operator-=(const float& f) {
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	glm::vec3 toVec3() const
	{
		glm::vec3 r;
		r.x = float(x);
		r.y = float(y);
		r.z = float(z);
		return r;
	}

	Pos ceil() const
	{
		Pos p;
		p.x = p_type(::ceil(x));
		p.y = p_type(::ceil(y));
		p.z = p_type(::ceil(z));
		return p;
	}

	Pos floor()
	{
		Pos p;
		p.x = p_type(::floor(x));
		p.y = p_type(::floor(y));
		p.z = p_type(::floor(z));
		return p;
	}

	Pos round()
	{
		Pos p;
		p.x = p_type(::round(x));
		p.y = p_type(::round(y));
		p.z = p_type(::round(z));
		return p;
	}
	dtype x;
	dtype y;
	dtype z;
};
template <typename T>
DPos operator*(const DPos& p, const T r) {
	DPos n;
	n.x = p.x * r;
	n.y = p.y * r;
	n.z = p.z * r;
	return n;
}


Pos operator+(const Pos& l, const Pos& r);
Pos operator-(const Pos& l, const Pos& r);
DPos operator+(const DPos& l, const DPos& r);
DPos operator-(const DPos& l, const DPos& r);
/*
struct IdData
{
	bid id;
	unsigned char data = 0;
};

bool operator<(const IdData& l, const IdData& r) {
	if (l.id == r.id)
		return l.data < r.data;
	return l.id < r.id;
}
bool operator>(const IdData& l, const IdData& r) {
	if (l.id == r.id)
		return l.data > r.data;
	return l.id > r.id;
}*/
bool operator==(const glm::vec3& pos, const DPos& rhs);
bool operator!=(const glm::vec3& pos, const DPos& rhs);
bool operator==(const DPos& pos, const DPos& rhs);
bool operator!=(const DPos& pos, const DPos& rhs);
bool operator==(const Pos& x, const Pos& y);