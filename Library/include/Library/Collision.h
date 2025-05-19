#pragma once
#include "Math.h"
const double eps = 1e-16;

struct CollisionModel {
	virtual bool isCollision(const CollisionModel&)const = 0;
	virtual ~CollisionModel() {}
	virtual const Vector3& getOrigin()const = 0;
};

struct Cuboid:public CollisionModel {
	Vector3 center;
	Vector3 half;
	Cuboid() {}
	Cuboid(const Vector3 &center,const Vector3 &half) :center(center),half(half){}
	virtual const Vector3& getOrigin()const override {
		return center;
	}

	bool isCollision( const CollisionModel& o)const override{
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
	Sphere() {}
	Sphere(const Vector3& v, double r) :center(v), r(r) {}
	virtual const Vector3& getOrigin()const override {
		return center;
	}
	bool isCollision(const CollisionModel& o)const override {
		const Sphere& b = dynamic_cast<const Sphere&> (o);
		return center.squareDist(b.center) < (r+b.r)*(r+b.r);
	}
};