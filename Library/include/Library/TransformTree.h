#pragma once
#include <string>
#include <stack>
class Light;
class Element;
class Resource;
class TransformNode;
class Matrix44;
using std::string;
using std::stack;
class TransformTree {
public:
	TransformTree(const Element* e, Resource* resource);
	TransformNode* getNode(const string &name);
	void draw(const Matrix44& pv, const Matrix44& parent_wm, Light* light) const;
protected:
	TransformNode* buildTree(const Element* e, Resource* resource);
private:
	string name;
	TransformNode* root_; // 动态建树
};