#pragma once
#include<vector>
#include <string>
class Curve;
class Vector3;
using std::string;
using std::vector;
class Element;
class AnimationNode {
public:
	AnimationNode(const Element* e);
	void update(Vector3& translation, Vector3& rotation, Vector3& scale,double time);
	const string& getName() const;
	~AnimationNode();
private:
	string name;
	vector<Curve*> curves_;
};