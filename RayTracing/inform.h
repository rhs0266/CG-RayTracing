#pragma once
#include "material.h"
#include "rhs_math.h"
struct Inform {
	position normal; // normal vector
	position p; // 3D point
	Material mat;
	double n; // snell's coeffiecient
	double reflect; // reflection
	double refract; // refraction
	Inform() {};
	Inform(bool flag) {
		for (int i = 0;i < 3;i++) mat.diffuse[i] = 1;
	}
};