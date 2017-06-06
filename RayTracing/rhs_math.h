#pragma once
#include <cmath>
#include <iostream>
#define FOR(i,n,m) for (int i=(n);i<=(m);i++)
static double eps = 0.0001;
using namespace std;

struct position{
    double p[3];
    position(){};
	position(double *_p) {
		p[0] = _p[0], p[1] = _p[1], p[2] = _p[2];
	}
	position(double p0, double p1, double p2) {
		p[0] = p0, p[1] = p1, p[2] = p2;
	}
    position operator*(position const& rhs){
        return position(p[1]*rhs.p[2] - p[2]*rhs.p[1],
                        p[2]*rhs.p[0] - p[0]*rhs.p[2],
                        p[0]*rhs.p[1] - p[1]*rhs.p[0]);
    }
    position operator+(position const& rhs){
        return position(p[0]+rhs.p[0], p[1]+rhs.p[1], p[2]+rhs.p[2]);
    }
    position operator-(position const& rhs){
        return position(p[0]-rhs.p[0], p[1]-rhs.p[1], p[2]-rhs.p[2]);
    }
    double operator%(position const& rhs){
        return p[0]*rhs.p[0] + p[1]*rhs.p[1] + p[2]*rhs.p[2];
    }
    position operator/(double const& x){
        return position(p[0]/x, p[1]/x, p[2]/x);
    }
    position operator*(double const& x){
        return position(p[0]*x, p[1]*x, p[2]*x);
    }
};

double norm(position a){
    return sqrt(a.p[0]*a.p[0] + a.p[1]*a.p[1] + a.p[2]*a.p[2]);
}

double angle(position &pv, position &v){ // return value is radian
    position cross = pv*v;
    return atan2(norm(cross)/norm(pv)/norm(v), (pv%v)/norm(pv)/norm(v));
}

struct matrix{
    double a[4][4];
    double v[16];
    matrix(){
        FOR (i,0,3) FOR (j,0,3) a[i][j]=0;
        FOR (i,0,3) a[i][i]=1;
    };
    matrix(double _a[4][4]){
        FOR (i,0,3) FOR (j,0,3) a[i][j]=_a[i][j];
    }
    matrix operator*(matrix const& rhs){
        matrix res=matrix();
        FOR (i,0,3) FOR (j,0,3){
            res.a[i][j]=0;
            FOR (k,0,3) res.a[i][j]+=a[i][k]*rhs.a[k][j];
        }
        return res;
    }
    void vec(){
        int t=0;
        FOR (i,0,3) FOR (j,0,3) v[t++]=a[i][j];
    }
    void print(){
        FOR (i,0,15) std::cout << v[i];
        std::cout << "\n";
    }
};

// reference at Jehee Lee math library
struct quater{
    double p[4];
    quater(){
        p[0]=1; FOR (i,1,3) p[i]=0;
    };
    quater(double p0, double p1, double p2, double p3){
        p[0]=p0, p[1]=p1, p[2]=p2, p[3]=p3;
    }
    quater(double _p[4]){
        FOR (i,0,3) p[i]=_p[i];
    }
    quater(double p0, position v){
        p[0]=p0, p[1]=v.p[0], p[2]=v.p[1], p[3]=v.p[2];
    }
    quater inverse(){
        return quater(p[0],-p[1],-p[2],-p[3]);
    }
    quater operator*(double x){
        return quater(x*p[0], x*p[1], x*p[2], x*p[3]);
    }
    quater operator*(quater const& rhs){
        quater c;
        position v1=position(p[1],p[2],p[3]);
        position v2=position(rhs.p[1],rhs.p[2],rhs.p[3]);
        double w1=p[0], w2=rhs.p[0];
        return quater(w1*w2-v1%v2, v2*w1 + v1*w2 + v1*v2);
    }
    matrix toMatrix(){
        matrix m;
        m.a[0][0]=p[0]*p[0] + p[1]*p[1] - p[2]*p[2] - p[3]*p[3];
        m.a[1][0]=2*p[1]*p[2] - 2*p[0]*p[3];
        m.a[2][0]=2*p[1]*p[3] + 2*p[0]*p[2];
        m.a[3][0]=0;
    
        m.a[0][1]=2*p[1]*p[2] + 2*p[0]*p[3];
        m.a[1][1]=p[0]*p[0] - p[1]*p[1] + p[2]*p[2] - p[3]*p[3];
        m.a[2][1]=2*p[2]*p[3] - 2*p[0]*p[1];
        m.a[3][1]=0;

        m.a[0][2]=2*p[1]*p[3] - 2*p[0]*p[2];
        m.a[1][2]=2*p[2]*p[3] + 2*p[0]*p[1];
        m.a[2][2]=p[0]*p[0] - p[1]*p[1] - p[2]*p[2] + p[3]*p[3];
        m.a[3][2]=0;

        m.a[0][3]=0;
        m.a[1][3]=0;
        m.a[2][3]=0;
        m.a[3][3]=1;
        return m;
    }
    double getTheta(){
        position v = position(p[1],p[2],p[3]);
        double t = atan2(norm(v),p[0]);
        return t;
    }
    position getVec(){
		position v = position(p[1],p[2],p[3]);
		if (norm(v)>1e-3) v = v / norm(v);
        return v;
    }
};

position calc_rotate(quater Q, position _P){
    quater P = quater(0, _P);
    P=(Q*P)*Q.inverse();
    return position(P.p[1], P.p[2], P.p[3]);
}

position qlog(quater q){
	return q.getVec() * q.getTheta();
}

quater qexp(position p){
	double theta = norm(p);
	if (theta>1e-3) p = p/theta;
	return quater(cos(theta), p*sin(theta));
}

quater slerp(double t, quater q1, quater q2){
	return q1 * qexp(qlog(q1.inverse() * q2) * t);
}
