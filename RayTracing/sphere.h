#pragma once
#include "rhs_math.h"
#include "material.h"
struct Sphere {
	position center;
	double radius, reflect, refract, n;
	Material mat;
	bool wall;
	BMP *texture, *bump;
	Sphere() {};
	Sphere(position _c, double _r) {
		center = _c;
		radius = _r;
		reflect = refract = n = 0;
		mat = silver;
		wall = false;
	}
	Sphere(position _c, double _r, double _reflect, double _refract, double _n, Material _mat) {
		center = _c;
		radius = _r;
		reflect = _reflect;
		refract = _refract;
		n = _n;
		mat = _mat;
		texture = NULL;
		bump = NULL;
		wall = false;
	}
	Sphere(position _c, double _r, double _reflect, double _refract, double _n, Material _mat, BMP *_texture) {
		center = _c;
		radius = _r;
		reflect = _reflect;
		refract = _refract;
		n = _n;
		mat = _mat;
		texture = _texture;
		bump = NULL;
		wall = false;
	}
	Sphere(position _c, double _r, double _reflect, double _refract, double _n, Material _mat, BMP *_texture, BMP *_bump) {
		center = _c;
		radius = _r;
		reflect = _reflect;
		refract = _refract;
		n = _n;
		mat = _mat;
		texture = _texture;
		bump = _bump;
		wall = false;
	}
	Sphere(position _c, double _r, bool flag) {
		center = _c;
		radius = _r;
		reflect = refract = n = 0;
		mat = silver;
		wall = flag;
	}
	Sphere(position _c, double _r, bool flag, Material _mat) {
		center = _c;
		radius = _r;
		reflect = refract = n = 0;
		mat = _mat;
		wall = flag;
	}
};