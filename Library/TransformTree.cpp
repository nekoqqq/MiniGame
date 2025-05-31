#include "pch.h"
#include "include/Library/TransformTree.h"
#include "include/Library/Resource.h"
#include "include/Library/Math.h"
#include <cassert>

TransformTree::TransformTree(const Element* e, Resource* resource) {
	assert(e->getTagName() == "TransformTree");
	assert(e->getChildren().size() == 1);
	root_ = buildTree(e, resource);
	this->name = e->getAttr("name");
	GameLib::cout << "create transform tree: " << name.c_str() << GameLib::endl;
}
TransformNode* TransformTree::getNode(const string& name) {
	if (!root_)
		return nullptr;
	stack<TransformNode*> st;
	st.push(root_);
	while (!st.empty()) {
		TransformNode* cur = st.top();
		if (cur->getName() == name)
			return cur;
		st.pop();
		for (int i = 0; i < cur->getChildren().size(); i++) {
			st.push(cur->getChild(i));
		}
	}
	return nullptr;
}
void TransformTree::draw(const Matrix44& pv, const Matrix44& parent_wm, Light* light)const{
	root_->draw(pv, parent_wm, light);
}
TransformNode* TransformTree::buildTree(const Element* e, Resource* resource) {
	TransformNode* node = new TransformNode();
	string name = e->getAttr("name");
	string painter = e->getAttr("painter");
	Vector3 translation = Element::converToVector(e->getAttr("translation"));
	Vector3 rotation = Element::converToVector(e->getAttr("rotation"));
	node->setName(name);
	node->setTranslation(translation);
	node->setRotation(rotation);
	node->setPainter(resource->getPainter(painter));
	for (auto& child : e->getChildren()) {
		node->addChild(buildTree(child, resource));
	}
	return node;
}
