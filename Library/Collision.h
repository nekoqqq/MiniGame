#pragma once
#include "Math.h"
struct Cuboid {
	double x;
	double y;
	double z;
	double half_x;
	double half_y;
	double half_z;
	void set(const Vector3& center, const Vector3& half) {
		x = center.x;
		y = center.y;
		z = center.z;
		half_x = half.x;
		half_y = half.y;
		half_z = half.z;
	}

	bool isCollision(const Cuboid& b)const {
		double al = x - half_x;
		double ar = x + half_x;
		double bl = b.x - b.half_x;
		double br = b.x + b.half_x;
		if (ar < bl || al > br) // 浮点数不可以用=比较
			return false;
		double at = y - half_y;
		double ab = y + half_y;
		double bt = b.y - b.half_y;
		double bb = b.y + b.half_y;
		if (ab < bt || at > bb)
			return false;
		double au = z - half_z;
		double af = z + half_z;
		double bu = b.z - b.half_z;
		double bf = b.z + b.half_z;
		if (af < bu || au > bf)
			return false;
		return true;

	}
};