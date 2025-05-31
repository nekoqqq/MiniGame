#include "pch.h"
#include <cassert>
#include "include/Library/AnimationNode.h"
#include "include/Library/Curve.h"
#include "include/Library/Resource.h"

AnimationNode::AnimationNode(const Element* e) {
	assert(e->getTagName() == "Node");
	for (auto& child : e->getChildren()) {
		curves_.push_back(new Curve(child));
	}
	this->name = e->getAttr("name");
	GameLib::cout << "create animation node: " << name.c_str() << GameLib::endl;
}
void AnimationNode::update(Vector3& translation, Vector3&rotation,Vector3&scale, double time)
{
	for (int i = 0; i < curves_.size(); i++) {
		double value = curves_[i]->get(time);
		switch (curves_[i]->getType())
		{
			case Curve::ROTATION_X:
				rotation.x = value;
				break;
			case Curve::ROTATION_Y:
				rotation.y = value;
				break;
			case Curve::ROTATION_Z:
				rotation.z = value;
				break;
			case Curve::TRANSLATION_X:
				translation.x = value;
				break;
			case Curve::TRANSLATION_Y:
				translation.y = value;
				break;
			case Curve::TRANSLATION_Z:
				translation.z = value;
				break;
			case Curve::SCALE_X:
				scale.x = value;
				break;
			case Curve::SCALE_Y:
				scale.y = value;
				break;
			case Curve::SCALE_Z:
				scale.z = value;
				break;
			default:
				break;
			}
	}
}
const string& AnimationNode::getName() const {
	return name;
}
AnimationNode::~AnimationNode()
{
	for (int i = 0; i < curves_.size(); i++) {
		delete curves_[i];
		curves_[i] = nullptr;
	}
	curves_.clear();
}

