#pragma once

template<class T>
class Smoother
{
private:
	T coef;
	T current;
public:
	Smoother(T c, T cur):
		coef(c),
		current(cur)
	{
		
	}
	T next(T val)
	{
		current += (val - current) * coef;
		return current;
	}
};