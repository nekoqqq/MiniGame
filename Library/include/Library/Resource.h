#pragma once
#include "Model.h"
#include "Missle.h"
#include "Mecha.h"
class Resource
{
public:
	Resource(const char* file_name) {
		XMLParser xml_parser(file_name);
		const Element* root = xml_parser.getRoot();

		for (auto& child : root->getChildren()) {
			const string& tag_name = child->getTagName();
			const string& name = child->getAttr("name");
			if (name == "") // 没有name的被忽略了
				continue;
			if (tag_name == "VertexBuffer") {
				vbs[name] = new VertexBuffer(child);
			}
			else if (tag_name == "IndexBuffer") {
				ibs[name] = (new IndexBuffer(child));
			}
			else if (tag_name == "Texture") {
				textures[name] = new Texture(child);
			}
		}
		// Painter依赖他们
		for (auto& child : root->getChildren()) {
			if (child->getTagName() == "Painter") {
				const string& name = child->getAttr("name");
				const string& vb_name = child->getAttr("vertexBuffer");
				const string& ib_name = child->getAttr("indexBuffer");
				const string& texture_name = child->getAttr("texture");
				const string& blend = child->getAttr("blend");
				Framework::BlendMode blend_mode = Framework::BLEND_OPAQUE;
				if (blend == "linear") {
					blend_mode = Framework::BLEND_LINEAR;
				}
				else if (blend == "additive")
					blend_mode = Framework::BLEND_ADDITIVE;
				bool is_ztest = true;
				Painter* painter = new Painter(vbs[vb_name], ibs[ib_name], textures[texture_name], is_ztest, blend_mode);
				painters[name] = painter;
				const string& origin = child->getAttr("origin");
				vector<double> origin_array = Element::converToArray<double>(origin);
				if (origin_array.empty()) {
					origin_array = vector<double>{ 0.0,0.0,0.0 };
				}
				origins[name] = new Vector3(origin_array[0], origin_array[1], origin_array[2]);
			}
		}
	}
	~Resource() {
		for (auto& kv : vbs)
			delete kv.second;
		for (auto& kv : ibs)
			delete kv.second;
		for (auto& kv : textures)
			delete kv.second;
		for (auto& kv : painters)
			delete kv.second;
		for (auto& kv : origins)
			delete kv.second;
	}
	Model* createModel(Model::Type type, CollisionModel::Type collision_type, const string& name) {
		Model* new_model = nullptr;
		if (painters.count(name)) {
			Painter* p = painters[name];
			const Vector3& origin = *origins[name];
			if (type == Model::PLAYER)
				new_model = new Mecha(type, origin, p, collision_type); // 这里必须让他一开始的坐标大于0，设置碰撞中心比地面高一点，不然由于和地面重合，导致无法跳起来
			else if (type == Model::ENEMY)
				new_model = new Mecha(type, origin, p, collision_type);
			else if (type == Model::STAGE)
				new_model = new Stage(type, p, collision_type);
			else if (type == Model::AXIS)
				new_model = new Axis(type, p, collision_type);
			else if (type == Model::MISSLE)
				new_model = new Missle(type, origin, p, collision_type, Matrix44::identity());
		}
		return new_model;
	}
private:
	unordered_map<string, VertexBuffer*> vbs;
	unordered_map<string, IndexBuffer*> ibs;
	unordered_map<string, Texture*> textures;
	unordered_map<string, Painter*> painters;
	unordered_map<string, Vector3*> origins; // 各个物体位于世界坐标系中的坐标

};
