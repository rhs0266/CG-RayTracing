#include "rhs_polygonset.h"
#include "material.h"
#include "light.h"
#include "sphere.h"
#include "inform.h"
#include "objLoader.h"
#define SS 1
#define ROOM 1
#define PLAYGROUND 2
#define DOF 3
#define SOFTSHADOW 4
#define MOTIONBLUR 5
#define BUMP 6
#define BSPCHECK 7

#define Ia 1

char Spline[15]; // spline type can be "BSPLINE" or "CATMULL_ROM"
static int lod[10] = { 1,2,5,10,20 };
static int lod_idx = 2;
static double INF = 100000000;
static double DOFvalue = 200;
PolygonSet totalPS, totalTPS;
vector < Sphere > totalSphere;
vector < Light > totalLight;
float lightPos0[] = { 0.0f, 20.0f, 30.0f, 1.0f };
float lightPos1[] = { 70.0f, 10.0f, -30.0f, 1.0f };
float lightPos2[] = { 30.0f, 0.0f, -30.0f, 1.0f };

float backgroundR = 1, backgroundG = 1, backgroundB = 1;
Inform background = Inform(false);

bool testbit = true;
int version;

BMP SkyTexture, GroundTexture, WallpaperTexture, WorldmapTexture, BasketballTexture, SakuraTexture, BilliardTexture, Billiard8, Billiard3, Billiard7;
BMP Bump;

void Parsing(char *name) {
	char temp[1000];
	FILE *in = fopen(name, "r"), *out = fopen("input_parse.txt", "w");
	while (!feof(in)) {			
		fgets(temp, 1000, in);
		for (int i = 0; i<strlen(temp); i++) {
			if (temp[i] == '#') { fprintf(out, "\n"); break; }
			fprintf(out, "%c", temp[i]);
		}
	}
	fclose(in), fclose(out);
}

position Mix(position a, position b, double x, double y) { // a:b=x:y MIX IT!
	return a*x + b*y;
}

Inform Rayshooting(position p, position v, bool *find) {
	// p+t*v on polygon
	double min_t = INF;
	Inform inform = Inform();
	/* check polygon use BSP*/
	//*
	Polygon poly;
	bool success = false;

	//BSPTreeSearchRecursive(0, p, v, &poly, &success);
	BSPTreeSearchNonRecursive(p, v, &poly, &success);

	if (success) {
		position n = poly.NormalVector();
		position p0 = poly.points[0];
		double t = (n % (p0 - p)) / (n % v);
		min_t = t;
		inform.normal = poly.NormalVector();
		inform.mat = poly.material;
		inform.p = p + v*t;
		inform.reflect = poly.reflect;
		inform.refract = poly.refract;
		inform.n = poly.n;
		*find = true;
	}
	//*/

	/* check polygon in Texture PS */
	
	for (int i = 0; i < totalTPS.list.size();i++) {
		Polygon target = totalTPS.list[i];
		position n = target.NormalVector();
		position U = target.points[1] - target.points[0];
		position V = target.points[2] - target.points[0];
		position v0 = target.points[0];
		double t = (n % (v0 - p)) / (n%v);
		position pI = p + v * t;
		position W = pI - v0;
		double sI = ((U%V)*(W%V) - (V%V)*(W%U)) / ((U%V)*(U%V) - (U%U)*(V%V));
		double tI = ((U%V)*(W%U) - (U%U)*(W%V)) / ((U%V)*(U%V) - (U%U)*(V%V));
		if (-1e-8 <= sI && sI <= 1 && -1e-8 <= tI && tI <= 1 && sI + tI <= 1 && min_t > t && t > 1e-3) {
			inform.mat = target.material;
			if (target.texture != NULL) {
				pair<int, int> X;
				pair<int, int> TU, TV;
				TU.first = target.UV[1].first - target.UV[0].first;
				TU.second = target.UV[1].second - target.UV[0].second;
				TV.first = target.UV[2].first - target.UV[0].first;
				TV.second = target.UV[2].second - target.UV[0].second;

				X.first = target.UV[0].first + TU.first * sI + TV.first*tI;
				X.second = target.UV[0].second + TU.second* sI + TV.second*tI;

				X.first %= (*target.texture).TellWidth();
				X.second %= (*target.texture).TellHeight();

				BMP *texture = target.texture;

				FOR(j, 0, 3) inform.mat.ambient[j] = inform.mat.specular[j] = 0.0;
				inform.mat.shininess = 0;

				inform.mat.diffuse[0] = double((*texture).GetPixel(X.first, X.second).Red) / 255.0;
				inform.mat.diffuse[1] = double((*texture).GetPixel(X.first, X.second).Green) / 255.0;
				inform.mat.diffuse[2] = double((*texture).GetPixel(X.first, X.second).Blue) / 255.0;
			}
			inform.n = target.n;
			inform.reflect = target.reflect;
			inform.refract = target.refract;
			inform.p = p + v*t;
			inform.normal = target.normal[0] + (target.normal[1] - target.normal[0]) *sI + (target.normal[2] - target.normal[0])*tI; inform.normal = inform.normal / norm(inform.normal);
			*find = true;
			min_t = t;
		}
	}
	


	/* check sphere */
	for (int i = 0; i < totalSphere.size(); i++) {
		Sphere sp = totalSphere[i];
		position delta = sp.center - p;
		double a = v % delta;
		double b = delta%delta - sp.radius*sp.radius;
		// t^2 - 2at + b^2 = 0
		if (a*a - b < 1e-5) continue; // singular
		double root1 = a + sqrt(a*a - b);
		double root2 = a - sqrt(a*a - b);
		bool flag = false;
		if (min_t > root1 && root1 > 1e-3) {
			min_t = root1;
			flag = true;
		}
		if (min_t > root2 && root2 > 1e-3) {
			min_t = root2;
			flag = true;
		}
		if (flag){
			inform.p = p + v*min_t;
			if (sp.wall) inform.normal = inform.normal * -1.0;
			inform.mat = sp.mat;
			if (sp.texture!=NULL){
				position dir = inform.p - sp.center;
				quater q = quater(cos(PI/4), position(1,0,0) * sin(PI/4));
				quater p = quater(0, dir);
				quater res = q * p * q.inverse();
				FOR(tt, 0, 2) dir.p[tt] = res.p[tt+1];
				dir = dir / sp.radius;
				double theta, phi;
				theta = acos(dir.p[2]);
				phi = atan2(dir.p[1], dir.p[0]);
				while (theta < 0) theta += PI;
				while (theta > PI) theta -= PI;
				while (phi < 0) phi += PI * 2;
				while (phi > PI * 2) phi -= PI * 2;
				BMP *texture = sp.texture;
				int x = (int)(theta / PI * (double)(*texture).TellHeight());
				int y = (int)(phi / (PI * 2) * (double)(*texture).TellWidth());
				if (x < 0)
					x = 0;
				if (x >= (*texture).TellHeight())
					x = (*texture).TellHeight() - 1;
				if (y < 0)
					y = 0;
				if (y>= (*texture).TellWidth())
					y = (*texture).TellWidth() - 1;

				pair<int, int> X = make_pair(y, x);
				inform.mat.diffuse[0] = double((*texture).GetPixel(X.first, X.second).Red) / 255.0;
				inform.mat.diffuse[1] = double((*texture).GetPixel(X.first, X.second).Green) / 255.0;
				inform.mat.diffuse[2] = double((*texture).GetPixel(X.first, X.second).Blue) / 255.0;
			}
			if (sp.bump != NULL) {
				position dir = inform.p - sp.center;
				quater q = quater(cos(PI / 4), position(1, 0, 0) * sin(PI / 4));
				quater p = quater(0, dir);
				quater res = q * p * q.inverse();
				FOR(tt, 0, 2) dir.p[tt] = res.p[tt + 1];
				dir = dir / sp.radius;
				// x = sin(theta) cos(phi)
				// y = sin(theta) sin(phi)
				// z = cos(theta)
				double theta, phi;
				{
					theta = acos(dir.p[2]) * 2;
					phi = atan2(dir.p[1], dir.p[0]) * 2;
					while (theta < 0) theta += PI;
					while (theta > PI) theta -= PI;
					while (phi < 0) phi += PI * 2;
					while (phi > PI * 2) phi -= PI * 2;
				}
				// dx/dt = cos(theta) cos(phi)    dx/dp = -sin(theta) sin(phi)
				// dy/dt = cos(theta) cos(phi)    dy/dp = sin(theta) cos(phi)
				// dz/dt = -sin(theta)			  dz/dp = 0
				position dt = position(cos(theta)*cos(phi), cos(theta)*cos(phi), -sin(theta));
				position dp = position(-sin(theta)*sin(phi), sin(theta)*cos(phi), 0);
				double udut, udup;
				{
					BMP *texture = sp.bump;
					int x = (int)(theta / PI * (double)(*texture).TellHeight());
					int y = (int)(phi / (PI * 2) * (double)(*texture).TellWidth());
					int x2 = x + 1;
					int y2 = y + 1;
					x = Filter(x, 0, (*texture).TellHeight() - 1);
					y = Filter(y, 0, (*texture).TellWidth() - 1);
					x2 = Filter(x2, 0, (*texture).TellHeight() - 1);
					y2 = Filter(y2, 0, (*texture).TellWidth() - 1);
					udut = ((*texture).GetPixel(y, x2).Red - (*texture).GetPixel(y, x).Red)/255.0;
					udup = ((*texture).GetPixel(y2, x).Red - (*texture).GetPixel(y, x).Red)/255.0;

				}
				inform.normal = inform.p - sp.center; inform.normal = inform.normal / norm(inform.normal);
				inform.normal = inform.normal + (inform.normal * dt) * udut + (inform.normal * dp) * udup;
			}
			else {
				inform.normal = inform.p - sp.center; inform.normal = inform.normal / norm(inform.normal);
			}
			inform.reflect = sp.reflect;
			inform.refract = sp.refract;
			inform.n = sp.n;
			*find = true;
		}
	}
	return inform;
}

position BackwardRayTracing(position p, position v, int depth, bool *find) {
	double s = INF;
	Inform inform = Rayshooting(p, v, find);
	if (!(*find)) return position(0,0,0);
	if (depth > 5) {
		// TODO check
		*find = false;
		return position(0,0,0);
	}

	/* Shadow Ray */
	position amb = position(0, 0, 0);
	position spe = position(0, 0, 0);
	double Il = 0.0;
	for (int i = 0;i < totalLight.size();i++) {
		Light light = totalLight[i];
		position vec;
		if (light.type == POINT) {
			vec = light.pos - inform.p; vec = vec / norm(vec);
		}
		else if (light.type == DIRECTION) {
			vec = light.vec *(-1);
		}
		if (inform.normal % vec < 0) continue; // back-face
		double fraction;
		if (version == SOFTSHADOW) {
			double delta = 1.0 / double(SS*SS*SS);
			double r = light.radius;
			fraction = 0.0;
			FOR(t1, 0, SS - 1) FOR(t2, 0, SS - 1) FOR (t3,0,SS-1){
				vec = light.pos - inform.p;
				vec = vec + position(t1 - SS / 2, t2 - SS / 2, t3 - SS / 2) * r;
				vec = vec / norm(vec);
				
				bool flag = false;
				Inform shadowRay = Rayshooting(inform.p, vec, &flag);
				if (flag && (light.type == POINT && norm(inform.p - shadowRay.p) < norm(inform.p - light.pos)))
					continue;
				if (t1 == SS / 2 && t2 == SS / 2 && t3 == SS / 2) {
					//fraction = 1.0;
				}
				fraction += delta;
			}
			if (fraction > 1) fraction = 1.0;
		}
		else {
			fraction = 1.0;
			bool flag = false;
			Inform shadowRay = Rayshooting(inform.p, vec, &flag);
			if (flag && (light.type == POINT && norm(inform.p - shadowRay.p) < norm(inform.p - light.pos))) continue;
		}
		Il += light.Il * (inform.normal % vec) * fraction;
		{
			position N = inform.normal; N = N / norm(N);
			position L = vec;
			position R = N*(2 * (L%N)) - L;
			position V = eye - inform.p; V = V / norm(V);
			if (R%V > 0) {
				spe = spe + position(inform.mat.specular) * light.Il * pow((double)(R % V), double(inform.mat.shininess)) * fraction;
			}
		}
	}
	amb = amb + position(inform.mat.ambient) * Ia;
	

	/* reflect */
	position dif_reflect = position(0, 0, 0), dif_refract = position(0, 0, 0);
	bool find_reflect = false, find_refract = false;
	position N = inform.normal; N = N / norm(N);
	position L = v * (-1.0); L = L / norm(L);
	position R = N*(2 * (L%N)) - L;
	position T;
	position V = eye - p;
	position value_reflect=position(0,0,0), value_refract=position(0,0,0);

	if (L%N > 0 && inform.reflect > 1e-3) {
		find_reflect = false;
		value_reflect = BackwardRayTracing(inform.p, R, depth + 1, &find_reflect);
		if (!find_reflect) {
			value_reflect = position(0, 0, 0);
		}
	}

	/* refract */
	if (inform.refract > 1e-3) {
		find_refract = false;
		if (L%N < 0)
			N = N*(-1.0);
		double coeffN = inform.n;
		if (N%inform.normal < 0)
			coeffN = 1.0 / inform.n;  // 안에서 밖으로 나가는 ray
			
		double cosI = N%L; double sinI = sqrt(1-cosI*cosI);
		double sinR = coeffN * sinI;
		if (abs(sinR) < 1 - 1e-6) { // 전반사가 아닐 경우
			double cosR = sqrt(1 - sinR*sinR);
			T = N*(coeffN * cosI - cosR) - L*coeffN;
			if (depth == 1 && T.p[2]>0) {
				depth = 1;
			}
			value_refract = BackwardRayTracing(inform.p, T, depth + 1, &find_refract);

			if (!find_refract) {
				value_refract = position(0, 0, 0);
			}
		}
		else { // 전반사인 경우
			value_refract = BackwardRayTracing(inform.p, R, depth + 1, &find_refract);
			if (!find_refract) {
				value_refract = position(0, 0, 0);
			}
		}
	}

	// amb + dif + spe
	// dif = dif + refl + refr

	position dif = position(0, 0, 0);
	position matDiffuse = position(inform.mat.diffuse);
	//if (!testbit) printf("%.3lf %.3lf %.3lf\n",matDiffuse.p[0],matDiffuse.p[1],matDiffuse.p[2]);
	dif = Mix(Mix(matDiffuse * Il, value_reflect, 1 - inform.reflect, inform.reflect), value_refract, 1 - inform.refract, inform.refract);
	if (!testbit) printf("%.3lf %.3lf %.3lf\n",dif.p[0],dif.p[1],dif.p[2]);

	//position result = amb + dif + spe;
	position result = dif + spe;
	return result;
}

void Test() {
	double camDist = height / 2.0 / tan((fov / 2.0)*PI / 180.0);
	BMP test;
	test.SetSize(width, height);
	test.SetBitDepth(32);
	for (int i = 720 * height / 1200; i < 1020 * height / 1200; i++) {
		for (int j = 320 * width / 1200; j < 850 * width / 1200; j++) {
			//for (int i = 0; i < height; i++){
				//for (int j = 0; j < width; j++) {
			RGBApixel rgba;
			//i = height / 2, j = width / 2;
			//j = 550; i = 480;

			position color = position(0, 0, 0);
			bool find = false;
			double len_pixel = 1.0 / SS;
			position move[3] = { position(0,0,0),position(45.0 / (double)(SS * 2),45.0 / (double)(SS * 2),0), position(0,-24.0 / (double)(SS * 2),0) };

			FOR(p1, 0, SS - 1) FOR(p2, 0, SS - 1) {
				position _eye, vec;
				if (version == DOF) {
					_eye = eye + position((p1 - SS / 2) * 1, (p2 - SS / 2) * 1, 0);
					vec = position((double)(height / 2 - i), (double)(j - width / 2), -camDist);
					vec = vec / norm(vec);
					position target = eye - vec * (DOFvalue / vec.p[2]);
					vec = target - _eye; vec = vec / norm(vec);
				}
				else {
					_eye = eye;
					vec = position((double)(height / 2 - i) - len_pixel * p1, (double)(j - width / 2) + len_pixel * p2, -camDist);
					vec = vec / norm(vec);
				}

				if (version == MOTIONBLUR) {
					vector<Sphere> backup = totalSphere;
					for (int k = 0; k < totalSphere.size(); k++) {
						Sphere sp = totalSphere[k];
						sp.center = sp.center + move[k] * max(p1, p2) * 2;
						totalSphere[k] = sp;
					}
					color = color + BackwardRayTracing(_eye, vec, 0, &find) / (SS*SS);
					totalSphere = backup;
				}
				else {
					color = color + BackwardRayTracing(_eye, vec, 0, &find) / (SS*SS);
				}
			}

			if (!find) {
				rgba.Red = backgroundR;
				rgba.Green = backgroundG;
				rgba.Blue = backgroundB;
			}
			else {
				rgba.Red = Filter(color.p[0] * 255, 0, 255);
				rgba.Green = Filter(color.p[1] * 255, 0, 255);
				rgba.Blue = Filter(color.p[2] * 255, 0, 255);
			}

			test.SetPixel(j, i, rgba);
		}
		if (i % 20 == 0)
			test.WriteToFile("./test2.bmp");
	}
	test.WriteToFile("./test2.bmp");
}

void ObjectInit() {
	version = BSPCHECK;
	SkyTexture.ReadFromFile("./BMP/sky3.bmp");
	GroundTexture.ReadFromFile("./BMP/basketball ground.bmp");
	WallpaperTexture.ReadFromFile("./BMP/LittleBIGPlanet-wallpaper.bmp");
	WorldmapTexture.ReadFromFile("./BMP/world-map.bmp");
	BasketballTexture.ReadFromFile("./BMP/basketball.bmp");
	SakuraTexture.ReadFromFile("./BMP/Sakura.bmp");
	Billiard3.ReadFromFile("./BMP/billiard3.bmp");
	Billiard7.ReadFromFile("./BMP/billiard7.bmp");
	Billiard8.ReadFromFile("./BMP/billiard8.bmp");
	BilliardTexture.ReadFromFile("./BMP/billiard.bmp");
	Bump.ReadFromFile("./BMP/try1.bmp");
	if (version == PLAYGROUND){
		Polygon ground1 = Polygon(); ground1.setting(silver, 0.3, 0, 0);
		ground1.push(position(-200, -20, -100)); ground1.push(position(200, -20, 100)); ground1.push(position(200, -20, -100));
		ground1.setTexture(&GroundTexture);
		ground1.setCorrespondingPoint(0, 0, GroundTexture.TellWidth()-1, GroundTexture.TellHeight()-1, GroundTexture.TellWidth() - 1, 0);

		Polygon ground2 = Polygon(); ground2.setting(silver, 0.3, 0, 0);
		ground2.push(position(-200, -20, -100)); ground2.push(position(-200, -20, 100)); ground2.push(position(200, -20, 100));
		ground2.setTexture(&GroundTexture);
		ground2.setCorrespondingPoint(0, 0, 0, GroundTexture.TellHeight()-1, GroundTexture.TellWidth() - 1, GroundTexture.TellHeight() - 1);

		Polygon sky1 = Polygon(); sky1.setting(default_material, 0, 0, 0);
		sky1.push(position(-300, +300, -200)); sky1.push(position(+300, -300, -200)); sky1.push(position(+300, +300, -200));
		sky1.setTexture(&SkyTexture);
		sky1.setCorrespondingPoint(0, 0, SkyTexture.TellWidth() - 1, SkyTexture.TellHeight() - 1, SkyTexture.TellWidth()-1, 0);

		Polygon sky2 = Polygon(); sky2.setting(default_material, 0, 0, 0);
		sky2.push(position(-300, +300, -200)); sky2.push(position(-300, -300, -200)); sky2.push(position(+300, -300, -200));
		sky2.setTexture(&SkyTexture);
		sky2.setCorrespondingPoint(0, 0, 0, SkyTexture.TellHeight()-1, SkyTexture.TellWidth() - 1, SkyTexture.TellHeight() - 1);

		totalTPS.push(ground1); totalTPS.push(ground2);
		totalTPS.push(sky1); totalTPS.push(sky2);

		// Basketball
		totalSphere.push_back(Sphere(position(20, 10, 0), 30, 0, 0, 0, default_material, &BasketballTexture, &Bump));

		// HorseChess
		objLoader *objData = new objLoader();
		objData->load("chess_horse.obj");
		PolygonSet horsechess;
		horsechess.LoadOBJ(objData, position(0, 0, 0), 3, ruby);
		horsechess.calcAverageNormal();
		horsechess.Rotate(position(1, 0, 0), -PI / 2);
		horsechess.Rotate(position(0, 1, 0), -PI / 3);
		horsechess.Translate(position(-80, -20, 20));
		totalTPS.add(horsechess);

		// Refracting ball
		totalSphere.push_back(Sphere(position(40, 30, 50), 20, 0, 0.7, 1.1, default_material));
	}
	if (version == DOF) {
		totalSphere.push_back(Sphere(position(0, -100, -50), 45, 0, 0, 1.1, default_material, &WorldmapTexture));
		totalSphere.push_back(Sphere(position(0, 0, 0), 45, 0, 0, 1.1, default_material, &WorldmapTexture));
		totalSphere.push_back(Sphere(position(0, 70, 50), 45, 0, 0, 1.1, default_material, &WorldmapTexture));
	}
	if (version == SOFTSHADOW) {
		totalLight.clear();
		totalLight.push_back(Light(POINT, position(0, 200, 0), position(0, -1, 0), 1, 40));

		totalSphere.push_back(Sphere(position(0, 0, 0), 30, 0, 0, 0, default_material));

		Polygon ground1 = Polygon(); ground1.setting(silver, 0.3, 0, 0);
		ground1.push(position(-200, -40, -100)); ground1.push(position(200, -40, 100)); ground1.push(position(200, -40, -100));
		ground1.setTexture(&GroundTexture);
		ground1.setCorrespondingPoint(0, 0, GroundTexture.TellWidth() - 1, GroundTexture.TellHeight() - 1, GroundTexture.TellWidth() - 1, 0);

		Polygon ground2 = Polygon(); ground2.setting(silver, 0.3, 0, 0);
		ground2.push(position(-200, -40, -100)); ground2.push(position(-200, -40, 100)); ground2.push(position(200, -40, 100));
		ground2.setTexture(&GroundTexture);
		ground2.setCorrespondingPoint(0, 0, 0, GroundTexture.TellHeight() - 1, GroundTexture.TellWidth() - 1, GroundTexture.TellHeight() - 1);

		totalTPS.push(ground1); totalTPS.push(ground2);
	}
	if (version == MOTIONBLUR) {
		totalSphere.push_back(Sphere(position(70, -20, 0), 30, 0, 0, 0, plastic, &Billiard3));
		totalSphere.push_back(Sphere(position(-50, -70, 0), 30, 0, 0, 0, plastic, &Billiard7));
		totalSphere.push_back(Sphere(position(0, 70, 0), 30, 0, 0, 0, plastic, &Billiard8));

		Polygon Billiard1 = Polygon(); Billiard1.setting(default_material, 0, 0, 0);
		Billiard1.push(position(-300, +300, -30)); Billiard1.push(position(+300, -300, -30)); Billiard1.push(position(+300, +300, -30));
		Billiard1.setTexture(&BilliardTexture);
		Billiard1.setCorrespondingPoint(0, 0, BilliardTexture.TellWidth() - 1, BilliardTexture.TellHeight() - 1, BilliardTexture.TellWidth() - 1, 0);

		Polygon Billiard2 = Polygon(); Billiard2.setting(default_material, 0, 0, 0);
		Billiard2.push(position(-300, +300, -30)); Billiard2.push(position(-300, -300, -30)); Billiard2.push(position(+300, -300, -30));
		Billiard2.setTexture(&BilliardTexture);
		Billiard2.setCorrespondingPoint(0, 0, 0, BilliardTexture.TellHeight() - 1, BilliardTexture.TellWidth() - 1, BilliardTexture.TellHeight() - 1);

		totalTPS.push(Billiard1), totalTPS.push(Billiard2);
	}
	if (version == BUMP) {
		totalSphere.push_back(Sphere(position(0, 0, 60), 30, 0, 0, 0, default_material, &BasketballTexture, &Bump));
	}
	if (version == BSPCHECK) {
		// HorseChess
		objLoader *objData = new objLoader();
		objData->load("chess_horse.obj");
		PolygonSet horsechess;
		horsechess.LoadOBJ(objData, position(0, 0, 0), 3, ruby);
		horsechess.calcAverageNormal();
		horsechess.Rotate(position(1, 0, 0), -PI / 2);
		horsechess.Rotate(position(0, 1, 0), -PI / 3);
		horsechess.Translate(position(-80, -20, 20));
		totalPS.add(horsechess);
		BSP(totalPS);
		//totalTPS.add(horsechess);
	}
	totalTPS.calcAverageNormal();
}

void LightInit() {
	//totalLight.push_back(Light(POINT, position(eye)+position(0,0,20), position(0, 0, -1), 1, 0.2));
	totalLight.push_back(Light(POINT, position(eye) + position(30, 100, 0), position(0, 0, -1), 1, 0));
	//totalLight.push_back(Light(POINT, position(eye) + position(0, -10, 0), position(0, 0, -1), 0.1, 0.2));
	totalLight.push_back(Light(DIRECTION, position(eye) + position(0, 100, 0), position(0, -1, 0), 0.5, 0));
}

int main(int argc, char **argv) {
	LightInit();
	ObjectInit();
	Test();
	return 0;
}
