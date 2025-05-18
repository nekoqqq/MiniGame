#pragma once
#include "Math.h"
const double eps = 1e-16;
struct Cuboid {
	double x;
	double y;
	double z;
	double half_x;
	double half_y;
	double half_z;


	bool isCollision(const Cuboid& b)const {
		double al = x - half_x;
		double ar = x + half_x;
		double bl = b.x - b.half_x;
		double br = b.x + b.half_x;
		if (ar < bl+eps || al+eps > br) // 浮点数不可以用=比较,a==b => -eps <a-b<eps , ar<=bl return false, Thus ar<bl or ar==bl but bl > ar so -eps<bl -al
			return false;
		double at = y - half_y;
		double ab = y + half_y;
		double bt = b.y - b.half_y;
		double bb = b.y + b.half_y;
		if (ab < bt+eps|| at+eps > bb)
			return false;
		double au = z - half_z;
		double af = z + half_z;
		double bu = b.z - b.half_z;
		double bf = b.z + b.half_z;
		if (af+eps < bu || au +eps> bf )
			return false;
		return true;

	}
};