#pragma once
#include <vector>
#include <string>
using std::vector;
using std::string;
class Element;
class AnimationNode;
class AnimationTree {
public:
	AnimationTree(const Element* e);
	vector<AnimationNode*> getChildren() const;
	AnimationNode* getNode(const string& name) const;
	~AnimationTree();
private:
	vector<AnimationNode*> children_;
	string name;
};