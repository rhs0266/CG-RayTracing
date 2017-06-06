#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include "rhs_math.h"
#include "material.h"
#include <cmath>
#include <algorithm>
#include <string.h>
#include <Eigen/LU>
#include <Eigen/Dense>
#include "EasyBMP.h"

#define TETRA 1
#define HEXA 2
#define OCTA 3
#define DODECA 4
#define ICOSA 5

#define EPS 1e-2

FILE *out=stdout;

using namespace std;


typedef vector < position > vp;

/* set global variables */
int width=1200, height=1200;
int curMouseX, curMouseY;
int type, dist[6]={0,20,30,40,40,45};
bool leftButton = false;
bool rightButton = false;
bool seekFlag, showStdLine = false;
double PI = 3.141592f, r;
double Near = 1.0f, Far = 500.0f;
double trackballRadius;
double buf[701][701];
double fov = 65.0;
unsigned timeStep = 30;

/* For Playground */
/*
int width=1200, height=1200;
position eye = position(0.0f, 40.0f, 200.0f);
double fov = 65.0;
}*/

/* For FOD */
/*
int width=500, height=500;
position eye = position(0.0f, 0.0f, 200.0f);
double fov = 65.0;
}*/

/* vectors that makes the rotation and translation of the cube */
position eye = position(0.0f, 40.0f, 200.0f);
position ori = position(0.0f, 0.0f, 0.0f);
double rot[3] = { 0.0f, 0.0f, 0.0f };

void push3(double  *dest, position src) {
	for (int i = 0;i < 3;i++)dest[i] = src.p[i];
}

int Filter(int x,int L, int H) {
	if (x < L) return L;
	if (x > H) return H;
	return x;
}