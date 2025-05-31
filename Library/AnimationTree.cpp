#include "pch.h"
#include <cassert>
#include "include/Library/AnimationNode.h"
#include "include/Library/AnimationTree.h"
#include "include/Library/Resource.h"
AnimationTree::AnimationTree(const Element* e) {
	assert(e->getTagName() == "Animation");
	assert(e->getChildren().size() >0 &&"animation size not equal 1");

	for (auto& child : e->getChildren()) {
		children_.push_back(new AnimationNode(child));
	}
	this->name = e->getAttr("name");
	GameLib::cout << "create animation tree: " << name.c_str() << GameLib::endl;
}
vector<AnimationNode*> AnimationTree::getChildren() const {
	return children_;
}
AnimationNode* AnimationTree::getNode(const string& name) const
{
	for (auto& child : children_) {
		if (child->getName() == name)
			return child;
	}
	return nullptr;
}
AnimationTree::~AnimationTree()
{
	for (int i = 0; i < children_.size(); i++) {
		delete children_[i];
		children_[i] = nullptr;
	}
	children_.clear();
}
