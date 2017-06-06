#pragma once
#include "rhs_math.h"
#include "rhs_library.h"
#include "objLoader.h"
struct Polygon {
	vp points;
	vp normal;
	Material material;
	BMP *texture;
	vector < pair <int,int> > UV;
	double reflect, refract, n;
	bool reverse; // 점의 시계, 반시계 방향
	Polygon(){
		texture = NULL;
		reverse = false;
	};
	void push(position pos) {
		points.push_back(pos);
	}
	void push_n(position _normal) {
		normal.push_back(_normal);
	}
	void setting(Material mat, double _reflect, double _refract, double _n){
		material = mat;
		reflect = _reflect;
		refract = _refract;
		n = _n;
	}
	void setTexture(BMP *_texture){
		texture = _texture;
	}
	void setCorrespondingPoint(int t1, int s1, int t2, int s2, int t3, int s3){
		UV.push_back(make_pair(t1,s1));
		UV.push_back(make_pair(t2,s2));
		UV.push_back(make_pair(t3,s3));
	}
	position NormalVector() {
		position v = (points[0] - points[1]) * (points[2] - points[1]);
		v = v / norm(v);
		if (reverse) return v*(-1);
		return v;
	}
	double getArea() {
		double sum = 0;
		for (int i = 1; i < points.size() - 1; i++) {
			sum += abs(norm((points[i] - points[0])*(points[i + 1] - points[0]))) / 2.0;
		}
		return sum;
	}
};
struct PolygonSet {
	vector<Polygon> list;
	PolygonSet() {};
	void push(Polygon poly) {
		list.push_back(poly);
	}
	void add(PolygonSet rhs) {
		for (int i = 0; i < rhs.list.size(); i++)
			list.push_back(rhs.list[i]);
	}
	void Translate(position translate) {
		for (int i = 0; i < list.size(); i++) {
			Polygon poly = list[i];
			for (int j = 0; j < poly.points.size(); j++) {
				poly.points[j] = poly.points[j] + translate;
			}
			list[i] = poly;
		}
	}
	void Rotate(position axis, double angle) {
		axis = axis / norm(axis);
		quater q = quater(cos(angle / 2), axis*sin(angle / 2));
		for (int i = 0; i < list.size(); i++) {
			Polygon poly = list[i];
			for (int j = 0; j < poly.points.size(); j++) {
				quater p = quater(0, poly.points[j]);
				quater res = q * p * q.inverse();
				FOR (k,0,2) poly.points[j].p[k] = res.p[k+1];
			}
			for (int j = 0; j < poly.normal.size(); j++) {
				quater p = quater(0, poly.normal[j]);
				quater res = q * p * q.inverse();
				FOR(k, 0, 2) poly.normal[j].p[k] = res.p[k + 1];
			}
			list[i] = poly;
		}
	}
	void MakeCube(position translate, Material material, double reflect, double refract, double n, double len) {
		Polygon poly;
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, len) + translate); poly.push(position(len, len, -len) + translate); poly.push(position(len, -len, -len) + translate); poly.push(position(len, -len, len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(-len, len, len) + translate); poly.push(position(-len, -len, len) + translate); poly.push(position(-len, -len, -len) + translate); poly.push(position(-len, len, -len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, len) + translate); poly.push(position(len, -len, len) + translate); poly.push(position(-len, -len, len) + translate); poly.push(position(-len, len, len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, -len) + translate); poly.push(position(-len, len, -len) + translate); poly.push(position(-len, -len, -len) + translate); poly.push(position(len, -len, -len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, len) + translate); poly.push(position(-len, len, len) + translate); poly.push(position(-len, len, -len) + translate); poly.push(position(len, len, -len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, -len, len) + translate); poly.push(position(len, -len, -len) + translate); poly.push(position(-len, -len, -len) + translate); poly.push(position(-len, -len, len) + translate); push(poly);
	}
	void MakeWall(position translate, Material material, double reflect, double refract, double n, double len) {
		Polygon poly;
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, len) + translate);  poly.push(position(len, -len, len) + translate);  poly.push(position(len, -len, -len) + translate);   poly.push(position(len, len, -len) + translate);  push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(-len, len, len) + translate); poly.push(position(-len, len, -len) + translate); poly.push(position(-len, -len, -len) + translate);  poly.push(position(-len, -len, len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, -len) + translate); poly.push(position(len, -len, -len) + translate); poly.push(position(-len, -len, -len) + translate);  poly.push(position(-len, len, -len) + translate); push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, len, len) + translate);  poly.push(position(len, len, -len) + translate);	poly.push(position(-len, len, -len) + translate); poly.push(position(-len, len, len) + translate);  push(poly);
		poly = Polygon(); poly.setting(material, reflect, refract, n); poly.push(position(len, -len, len) + translate); poly.push(position(-len, -len, len) + translate); poly.push(position(-len, -len, -len) + translate);  poly.push(position(len, -len, -len) + translate); push(poly);
	}
	void LoadOBJ(objLoader *objData, position pos, double scale, Material mat) {
		for (int i = 0; i < objData->faceCount; i++) {
			Polygon poly;
			position vertex, normal, color;
			FOR(j, 0, 2) {
				FOR(k, 0, 2) {
					vertex.p[k] = objData->vertexList[objData->faceList[i]->vertex_index[j]]->e[k];
					//normal.p[k] = objData->normalList[objData->faceList[i]->normal_index[j]]->e[k];
				}
				poly.push((vertex + pos) * scale);
				//poly.push_n(normal);
			}
			if (objData->faceList[i]->material_index != -1) {
				FOR(j, 0, 2)
					poly.material.ambient[j] = objData->materialList[objData->faceList[i]->material_index]->amb[j],
					poly.material.specular[j] = objData->materialList[objData->faceList[i]->material_index]->spec[j],
					poly.material.diffuse[j] = objData->materialList[objData->faceList[i]->material_index]->diff[j];
				poly.material.shininess = objData->materialList[objData->faceList[i]->material_index]->shiny;
				poly.n = objData->materialList[objData->faceList[i]->material_index]->refract_index;
			}
			poly.reflect = poly.refract = 0;
			poly.texture = NULL;
			poly.material = mat;
			poly.reverse = true;
			push(poly);
		}
	}
	void calcAverageNormal() {
		for (int i = 0; i < list.size();i++){
			FOR(j, 0, 2) {
				position p = list[i].points[j];
				position n = position(0, 0, 0);
				int count = 0;
				for (int k = 0; k < list.size();k++){
					FOR(l, 0, 2) {
						position q = list[k].points[l];
						if (norm(p - q) < 1e-3) {
							count++;
							position v = (list[k].points[(l + 1) % 3] - list[k].points[l]) * (list[k].points[(l + 2) % 3] - list[k].points[l]); v = v / norm(v);
							n = n + v;
						}
					}
				}
				n = n / (double)count;
				n = n / norm(n);

				//n = (list[i].points[(j + 1) % 3] - list[i].points[j]) * (list[i].points[(j + 2) % 3] - list[i].points[j]); n = n / norm(n);

				list[i].normal.push_back(n);
			}
		}
	}
};
 
struct BSPTree {
	Polygon poly;
	int leftNum, rightNum;
	BSPTree() {
		leftNum = rightNum = -1;
	}
	BSPTree(Polygon _poly) {
		poly = _poly;
		leftNum = rightNum = -1;
	}
};
BSPTree res[1000005];
int last[1000005];
int res_cnt;
int stack[1000005], stack_cnt;
int saveCnt;
 
int place(position p, position v, double d) {
	if (p%v + d > eps) return 1;
	if (p%v + d > -eps) return 0;
	return -1;
}
 
position InterceptPoint(position p1, position p2, position v, double d) {
	double t = (p1%v + d) / (p1%v - p2%v);
	return p1 + (p2 - p1)*t;
}

bool compareArea(Polygon A, Polygon B) {
	return A.getArea() > B.getArea();
}

int BSP(PolygonSet ps) {
	if (ps.list.size() == 0) {
		return -1;
	}
	sort(ps.list.begin(), ps.list.end(), compareArea);
	Polygon stdpoly = ps.list[0];
//	res.push_back(BSPTree(stdpoly));
	int here = res_cnt;
	res[res_cnt++]=BSPTree(stdpoly);
	PolygonSet leftSet = PolygonSet(), rightSet = PolygonSet();
	for (int t = 1; t < ps.list.size(); t++) {
		Polygon target = ps.list[t];
		Polygon left = Polygon(), right = Polygon();
		left = target; right = target;
		left.points.clear(); right.points.clear();
		
		position v = stdpoly.NormalVector(), p = stdpoly.points[0];
		int cnt = 0, leftcnt = 0, rightcnt = 0;
		for (int i = 0; i < target.points.size(); i++) {
			int i2 = (i + 1) % target.points.size();
			int q1 = place(target.points[i], v, -(p%v));
			int q2 = place(target.points[i2], v, -(p%v));
			if (q1 == 0 || q1 == 1) left.push(target.points[i]);
			else right.push(target.points[i]);
			//if (q1 == 0 || q1 == 2) right.push(target.points[i]);
			if (q1 * q2 == -1 || (q1 == 0 && q2 == 1) || (q1 == 0 && q2 == -1)) {
				if (q1!=0) left.push(InterceptPoint(target.points[i], target.points[i2], v, -(p%v)));
				right.push(InterceptPoint(target.points[i], target.points[i2], v, -(p%v)));
				cnt++;
			}
			if (q1 == 1) leftcnt++;
			if (q1 == -1) rightcnt++;
		}
		if (cnt == 2) {
			leftSet.push(left);
			rightSet.push(right);
		}
		else {
			if (leftcnt > rightcnt) leftSet.push(target);
			else rightSet.push(target);
		}
	}
	res[here].leftNum = BSP(leftSet);
	res[here].rightNum = BSP(rightSet);
	return here;
}

// Recursive
void BSPTreeSearchRecursive(int x, position p, position v, Polygon *polygon, bool *find) {
	if (x >= res_cnt) return;
	if (x == -1) return;
	Polygon poly = res[x].poly;
	position n = poly.NormalVector(), p0 = poly.points[0];
	if (place(p, n, -(p0%n)) != 1) {
		BSPTreeSearchRecursive(res[x].rightNum, p, v, polygon, find);
	}
	else {
		BSPTreeSearchRecursive(res[x].leftNum, p, v, polygon, find);
	}
	if (*find) return;
	if (abs(n%v) >= 1e-6) {
		double t = (n % (p0 - p)) / (n % v);
		if (t >= 1e-6) {
			int cnt1 = 0, cnt2 = 0;
			for (int i = 0; i < poly.points.size(); i++) {
				double c = ((poly.points[(i + 1) % poly.points.size()] - poly.points[i]) * (p + v*t - poly.points[i])) % n;
				if (-1e-6 < c && c < 1e-6) continue; // on the line
				if (c > 0) {
					cnt1++;
				}
				else {
					cnt2++;
				}
			}
			if (!(cnt1 > 0 && cnt2 > 0)) {
				*polygon = poly;
				*find = true;
				return;
			}
		}
	}
	if (place(p, n, -(p0%n)) != 1) {
		BSPTreeSearchRecursive(res[x].leftNum, p, v, polygon, find);
	}else{
		BSPTreeSearchRecursive(res[x].rightNum, p, v, polygon, find);
	}
}

// Non-Recursive
void BSPTreeSearchNonRecursive(position p, position v, Polygon *polygon, bool *find) {
	if (res_cnt == 0) return;
	saveCnt++;
	stack_cnt = 0;
	stack[++stack_cnt] = 0;
	while (stack_cnt) {
		int top = stack[stack_cnt];
		Polygon poly = res[top].poly;
		position n = poly.NormalVector(), p0 = poly.points[0];
		if (last[top] != saveCnt) {
			if (place(p, n, -(p0%n)) != 1) {
				if (res[top].rightNum!=-1) stack[++stack_cnt] = res[top].rightNum;
			}
			else {
				if (res[top].leftNum!=-1) stack[++stack_cnt] = res[top].leftNum;
			}
			last[top] = saveCnt;
			if (top!=stack[stack_cnt]) continue;
		}
		if (abs(n%v) >= 1e-6) {
			double t = (n % (p0 - p)) / (n % v);
			if (t >= 1e-6) {
				int cnt1 = 0, cnt2 = 0;
				for (int i = 0; i < poly.points.size(); i++) {
					double c = ((poly.points[(i + 1) % poly.points.size()] - poly.points[i]) * (p + v*t - poly.points[i])) % n;
					if (-1e-6 < c && c < 1e-6) continue; // on the line
					if (c > 0) {
						cnt1++;
					}
					else {
						cnt2++;
					}
				}
				if (!(cnt1 > 0 && cnt2 > 0)) {
					*polygon = poly;
					*find = true;
					return;
				}
			}
		}

		stack[stack_cnt--] = 0;
		if (place(p, n, -(p0%n)) != 1) {
			if (res[top].leftNum != -1) stack[++stack_cnt] = res[top].leftNum;
		}
		else {
			if (res[top].rightNum != -1) stack[++stack_cnt] = res[top].rightNum;
		}
	}
}


/*
void BSPTreePrint(int x,position eye){
	if (x>=res_cnt) return;
	if (x==-1) return;
	Polygon poly = res[x].poly;
	position v = poly.NormalVector(), p = poly.points[0];
	if (place(eye, v, -(p%v)) == 1){
		BSPTreePrint(res[x].rightNum, eye);
		glBegin(GL_POLYGON);
		FOR (i,0,poly.points.size()-1){
			glNormal3d(v.p[0],v.p[1],v.p[2]);
			{ // material
				float amb[4], dif[4], spe[4];
				FOR (j,0,3) amb[j]=poly.material.ambient[j], dif[j]=poly.material.diffuse[j], spe[j]=poly.material.specular[j];
				amb[3]=dif[3]=spe[3]=poly.Avalue;
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &poly.material.shininess);
			}
			glVertex3d(poly.points[i].p[0],poly.points[i].p[1],poly.points[i].p[2]);
		}
		glEnd();
		BSPTreePrint(res[x].leftNum, eye);
	}else{
		BSPTreePrint(res[x].leftNum, eye);
		glBegin(GL_POLYGON);
		FOR (i,0,poly.points.size()-1){
			glNormal3d(v.p[0],v.p[1],v.p[2]);
			{ // material
				float amb[4], dif[4], spe[4];
				FOR (j,0,3) amb[j]=poly.material.ambient[j], dif[j]=poly.material.diffuse[j], spe[j]=poly.material.specular[j];
				amb[3]=dif[3]=spe[3]=poly.Avalue;
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &poly.material.shininess);
			}
			glVertex3d(poly.points[i].p[0],poly.points[i].p[1],poly.points[i].p[2]);
		}
		glEnd();
		BSPTreePrint(res[x].rightNum, eye);
	}
}
*/
