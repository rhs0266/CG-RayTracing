#pragma once
#include "rhs_math.h"
typedef int LIGHTTYPE;
#define POINT 1
#define DIRECTION 2
#define AREA 3

struct Light {
	LIGHTTYPE type;
	position pos;
	position vec;
	double Il; // ������ �ϴ� ����� ����, ���Ŀ� ������ ����.... TODO
	double radius;
	Light() {};
	Light(LIGHTTYPE _type, position _pos, position _vec, double _Il, double _radius) :type(_type), pos(_pos), vec(_vec), Il(_Il), radius(_radius) {};
};