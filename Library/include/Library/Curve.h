#pragma once
#include<vector>
using std::vector;
class Element;
class Curve {
public:
	enum Type {
		ROTATION_X,
		ROTATION_Y,
		ROTATION_Z,
		TRANSLATION_X,
		TRANSLATION_Y,
		TRANSLATION_Z,
		SCALE_X,
		SCALE_Y,
		SCALE_Z
	};
	enum Interpolation_Type {
		NONE, // 无
		LINEAR, // 线性插值
		CUBIC, // 三次贝塞尔曲线插值
		HERMITE // 埃尔米特插值
	};
	Curve(const Element* e);
	Type getType() const;
	double get(double time) const;
	~Curve();
private:
	struct Data {
		double time;
		double value;
	};
	Type type_;
	Interpolation_Type interpolation_type_;
	vector<Data*> datas_;
};