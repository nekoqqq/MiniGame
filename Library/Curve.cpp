#include "pch.h"
#include <cmath>
#include <cassert>
#include "include/Library/Curve.h"
#include "include/Library/Resource.h"
Curve::Curve(const Element* e) {
	assert(e->getTagName() == "Curve");
	string type = e->getAttr("type");
	if (type == "rotation_x")
		type_ = ROTATION_X;
	else if (type == "rotation_y")
		type_ = ROTATION_Y;
	else if (type == "rotation_z")
		type_ = ROTATION_Z;
	else if (type == "translation_x")
		type_ = TRANSLATION_X;
	else if (type == "translation_y")
		type_ = TRANSLATION_Y;
	else if (type == "translation_z")
		type_ = TRANSLATION_Z;
	else if (type == "scale_x")
		type_ = SCALE_X;
	else if (type == "scale_y")
		type_ = SCALE_Y;
	else if (type == "scale_z")
		type_ = SCALE_Z;
	else
		assert(false && "Unknown curve type");
	string interpolation_type = e->getAttr("interpolation");
	if (interpolation_type == "none" ||interpolation_type =="")
		interpolation_type_ = NONE;
	else if (interpolation_type == "linear")
		interpolation_type_ = LINEAR;
	else if (interpolation_type == "cubic")
		interpolation_type_ = CUBIC;
	else if (interpolation_type == "hermite")
		interpolation_type_ = HERMITE;
	else
		assert(false && "Unknown interpolation type");
	for (auto& child : e->getChildren()) {
		double time = Element::convertToDouble(child->getAttr("time"));
		double value = Element::convertToDouble(child->getAttr("value"));
		datas_.push_back(new Data{ time, value });
	}
}
Curve::Type Curve::getType() const { return type_; }
double Curve::get(double time) const {
	time = fmod(time, datas_.back()->time); // 确保时间在循环范围内
	switch (interpolation_type_)
	{
	case Curve::NONE:
		{
			// 找到第一个大于等于time的点
			for (int i = 0; i < datas_.size(); i++) {
				if (datas_[i]->time >= time) {
					return datas_[i]->value;
				}
			}
			return datas_.back()->value; // 如果没有找到，返回最后一个点的值
		}
		break;
	case Curve::LINEAR:
		// 线性插值
		for (int i = 0; i < datas_.size() - 1; i++) {
			if (datas_[i]->time <= time && datas_[i + 1]->time >= time) {
				double t = (time - datas_[i]->time) / (datas_[i + 1]->time - datas_[i]->time);
				return datas_[i]->value + t * (datas_[i + 1]->value - datas_[i]->value);
			}
		}
		break;
	case Curve::CUBIC:
	{
		int beg = 0;
		int end = 0;
		for ( end = 0; end < datas_.size(); end++) {
			if (datas_[end]->time > time)
				break;
			beg = end;
		}
		double t0 = datas_[beg]->time;
		double t1 = datas_[end]->time;
		double p0 = datas_[beg]->value;
		double p1 = datas_[end]->value;
		double v0 = datas_[beg]->right_slope_;
		double v1 = datas_[end]->left_slope_;
		time = (time - t0) / (t1 - t0); // 归一化时间
		double a = 2.0 * (p0 - p1) + v0 + v1;
		double b = 3.0 * (p1 - p0) - (2.0 * v0) - v1;
		double r = a;
		r *= time;
		r += b;
		r *= time;
		r += v0;
		r *= time;
		r += p0;
		return r;
	}
		break;
	case Curve::HERMITE:
		break;
	default:
		break;
	}
}
Curve::~Curve()
{
	for (int i = 0; i < datas_.size(); i++) {
		delete datas_[i];
		datas_[i] = nullptr;
	}
	datas_.clear();
}