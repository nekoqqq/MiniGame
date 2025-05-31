#pragma once
#include <string>
#include <vector>
#include <stack>
#include <cassert>
#include <functional>
using std::function;
class Light;
class Element;
class Resource;
class TransformNode;
class Matrix44;
class AnimationNode;
class AnimationTree;
using std::string;
class TransformTree {
public:
	TransformTree(const Element* e, Resource* resource);
	TransformNode* getNode(const string &name);
	void setAnimation(AnimationTree* animation_tree);
	void update();
	void preOrder(function<void(TransformNode*)> func);
	void draw(const Matrix44& pv, const Matrix44& parent_wm, Light* light) const;
protected:
	TransformNode* buildTree(const Element* e, Resource* resource);
private:
	string name;
	TransformNode* root_; // 动态建树
	unsigned timer_; // 计时器
};