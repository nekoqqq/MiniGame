#pragma once
#include "Math.h"
const double eps = 1e-16;

struct CollisionModel {
	enum Type {
		NONE,
		CUBOID,
		SPHERE,
		TRIANGLE
	};
	CollisionModel(Type type):type(type) {}
	Type getType()const {
		return type;
	}
	virtual bool isCollision(const CollisionModel&)const = 0;
	virtual ~CollisionModel() {}
	virtual const Vector3& getOrigin()const = 0;
	Type type;
};

struct Cuboid:public CollisionModel {
	Vector3 center;
	Vector3 half;
	Cuboid():CollisionModel(CUBOID) {}
	Cuboid(const Vector3 &center,const Vector3 &half) :CollisionModel(type), center(center),half(half){
		type = CUBOID;
	}
	virtual const Vector3& getOrigin()const override {
		return center;
	}

	bool isCollision( const CollisionModel& o)const override{
		if (o.getType() != CUBOID)
			return false;
		const Cuboid &b = dynamic_cast<const Cuboid&>((o));
		double al = center.x - half.x;
		double ar = center.x + half.x;
		double bl = b.center.x - b.half.x;
		double br = b.center.x + b.half.x;
		if (ar < bl+eps || al+eps > br) // 浮点数不可以用=比较,a==b => -eps <a-b<eps , ar<=bl return false, Thus ar<bl or ar==bl but bl > ar so -eps<bl -al
			return false;
		double at = center.y - half.y;
		double ab = center.y + half.y;
		double bt = b.center.y - b.half.y;
		double bb = b.center.y + b.half.y;
		if (ab < bt+eps|| at+eps > bb)
			return false;
		double au = center.z - half.z;
		double af = center.z + half.z;
		double bu = b.center.z - b.half.z;
		double bf = b.center.z + b.half.z;
		if (af+eps < bu || au +eps> bf )
			return false;
		return true;

	}
};

struct Sphere:public CollisionModel {
	Vector3 center;
	double r;
	Sphere():CollisionModel(SPHERE) {}
	Sphere(const Vector3& v, double r) :CollisionModel(SPHERE), center(v), r(r) {}
	virtual const Vector3& getOrigin()const override {
		return center;
	}
	bool isCollision(const CollisionModel& o)const override {
		if (o.getType() != SPHERE)
			return false;
		const Sphere& b = dynamic_cast<const Sphere&> (o);
		return center.squareDist(b.center) < (r+b.r)*(r+b.r);
	}
};

struct Triangle:public CollisionModel {
	Vector3 points[3];
	Vector3 getNorm()const {
		const Vector3& AB = points[1] - points[0];
		const Vector3& AC = points[2] - points[0];
		const Vector3& n = AB.cross(AC);
		return n;
	}
	Triangle() :Triangle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }) {}
	Triangle(const Vector3& a, const Vector3& b, const Vector3& c):CollisionModel(TRIANGLE), points{ a,b,c } {}
	virtual const Vector3& getOrigin()const override {
		return points[0];
	}
	bool isCollision(const CollisionModel& o)const override { return false; }
	bool isCollision(const Vector3& origin, const Vector3& direction) const { // M的端点以及方向
		// 0:A 1:B 2:C
		const Vector3& M = origin;
		const Vector3& N = origin + direction;
		const Vector3& AB = points[1] - points[0];
		const Vector3& AC = points[2] - points[0];
		const Vector3& n = getNorm();
		const Vector3& AM = M - points[0];
		const Vector3& AN = N - points[0];
		const Vector3& MN = N - M; // 直线l
		// 先求出直线和平面的交点
		double ln = MN.dot(n);
		if (fabs(ln) < eps) // 在平面内和平面平行视为没有碰撞
			return false;
		double lambda = AN.dot(n) / ln;
		if (lambda < 0.0 || lambda > 1)
			return false;
		// 线段和平面的交点,求解线性方程组
		// a11*s+a12*t = b1, a21*s + a22*t = b2;
		const Vector3& AP = AM * lambda + AN * (1 - lambda);
		double a11 = AB.dot(AB);
		double a12 = AC.dot(AB);
		double a21 = a12;
		double a22 = AC.dot(AC);
		double b1 = AP.dot(AB);
		double b2 = AP.dot(AC);
		// 行列式a11*a22- a21*a12 一定不为0，因此分母不为0
		double det = a11 * a22 - a12 * a21;
		assert(fabs(det) > eps);
		double s = (b1 * a22 - b2 * a12) / det;
		if (s < 0.0 || s > 1)
			return false;
		double t = (a11 * b2 - a21 * b1) / det;
		if (t < 0 || s + t>1)
			return false;
		return true;
	}
};