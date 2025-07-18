#pragma once
#include <vector>
#include <array>
#include <cassert>
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "Math.h"
#include "Xml.h"
#include "Collision.h"
#include "AnimationTree.h"
#include "AnimationNode.h"
using std::min;
using std::max;

using namespace GameLib::Input;
using GameLib::Framework;
using std::vector;
using std::array;
namespace GameLib {
	class Texture;
}
enum Color {
	WHITE = 0xffffffff,
	RED = 0xffff0000,
	GREEN = 0xff00ff00,
	BLUE = 0xff0000ff,
	H0 = 0xff8f0000,
	H1 = 0xff008f00,
	H2 = 0xff00008f,
	Q0 = 0xff4f0000,
	Q1 = 0xff004f00,
	Q2 = 0xff00004f
};
struct Light
{
	Light(const Vector3& light_dir, const Vector3& light_color,const Vector3&ambient)
	{
		this->light_dir = light_dir;
		this->light_color = light_color;
		this->ambient = ambient;
	}
	void updateLight(const Vector3&new_dir)
	{
		light_dir = new_dir;
	}
	unsigned calculate(const Vector3& norm, unsigned diffuse_color)const
	{
		Vector3 diffuse = { (diffuse_color >> 16 & 0xff)/255.0, ((diffuse_color >> 8) &0xff)/255.0, (diffuse_color & 0xff)/255.0 };
		// 三角形表面法向量, n
		// diffuse 物体本身的性质,漫反射率,反射RGB三种颜色的强度,0到1,1最大, R
		double c = light_dir.dot(norm)/light_dir.norm();
		c = max(0.0, c);
		Vector3 color = light_color.elementMul(diffuse) * c + ambient; // RGB
		unsigned r = max(0.0, min(1.0, color.x))*255.0;
		unsigned g = max(0.0, min(1.0, color.y))*255.0;
		unsigned b = max(0.0, min(1.0, color.z))*255.0;
		return (0xff << 24) | (r << 16) | (g << 8) | b;
	}
	// 基础光照模型, I = I0*R*(n.l)/ d^2 +a ，由于距离较远，可以将分母忽略
	// 这里为了复用Vector3这个类库，所以设置了归一化的RGB值
	Vector3 light_dir; // 光线的方向向量，由物体指向光源, l
	Vector3 light_color; // 光源的性质,RGB三种颜色的强度,0到1,1最强，表示光的强度, I0
	Vector3 ambient; // 环境光，各个颜色的分量强度其实就是一个unsigned, a
};
class VertexBuffer {
public:
	VertexBuffer() {}
	VertexBuffer(int n) // 存了多少点的坐标
	{
		points_.resize(n);
		uvs_.resize(n);
		colors_.resize(n);
	}
	VertexBuffer(const vector<Vector3>& vertexes, const vector<array<double, 2>>& uvs, const vector<Color>& colors = {}) {
		points_ = vertexes;
		uvs_ = uvs;
		if (colors.empty()) {
			for (int i = 0; i < points_.size(); i++) {
				colors_.push_back(WHITE);
			}
		}
		else
			colors_ = colors;
	}
	VertexBuffer(const Element* e) {
		for (auto& child : e->getChildren()) {
			string raw = child->getAttr("position");
			vector<double> point = Element::converToArray<double>(raw);
			assert(point.size() == 3);
			points_.push_back({ point[0], point[1], point[2] });

			string raw_uv = child->getAttr("uv");
			vector<double> uv = Element::converToArray<double>(raw_uv);
			assert(uv.size() == 2);
			uvs_.push_back({ uv[0],uv[1] });

			string raw_color = child->getAttr("color");
			unsigned color = Element::convertHexToUnsigned(raw_color);
			colors_.push_back(static_cast<Color>(color));
		}
		this->name = e->getAttr("name");
		GameLib::cout << "create vertex buffer: " << name.c_str() << GameLib::endl;
	}
	~VertexBuffer()
	{
		GameLib::cout << "delete vertex buffer: " << name.c_str() << GameLib::endl;
	}
	int size()const {
		return points_.size();
	}
	void setInternal(const vector<Vector3>& vertexes, const vector<array<double, 2>>& uv) {
		points_ = vertexes;
		uvs_ = uv;
	}
	void setPoint(int i, Vector3& v) {
		assert(0 <= i && i < points_.size());
		points_[i] = v;
	}
	const Vector3 vertex(int i)const {
		assert(0 <= i && i < points_.size());
		return points_[i];
	}
	Vector3& vertex(int i) {
		return points_[i];
	}
	const array<double, 2> uv(int i)const {
		assert(0 <= i && i < points_.size());
		return uvs_[i];
	}
	array<double, 2>& uv(int i) {
		return uvs_[i];
	}
	Color& color(int i) {
		return colors_[i];
	}
	const Color& color(int i)const {
		return colors_[i];
	}

private:
	string name;
	vector<Vector3> points_;
	vector<array<double, 2>> uvs_;
	vector<Color> colors_;
};
class IndexBuffer {
public:
	IndexBuffer() {}
	IndexBuffer(int n) {
		indices.resize(n);
	}
	IndexBuffer(const vector<array<unsigned, 3>>& arr) {
		indices = arr;
	}
	IndexBuffer(const Element* e) {
		for (auto& child : e->getChildren()) {
			string raw = child->getAttr("indices");
			vector<unsigned> point = Element::converToArray<unsigned>(raw);
			assert(point.size() == 3);
			indices.push_back({ point[0], point[1], point[2] });
		}
		this->name = e->getAttr("name");
		GameLib::cout << "create index buffer: " << name.c_str() << GameLib::endl;
	}
	~IndexBuffer() {
		GameLib::cout << "delete index buffer: " << name.c_str() << GameLib::endl;
	}
	void setIndex(int i, array<unsigned, 3> arr) {
		assert(0 <= i && i < indices.size());
		indices[i] = arr;
	}
	int size()const {
		return indices.size();
	}
	const array<unsigned, 3> operator[](unsigned i)const {
		return indices[i];
	}
	array<unsigned, 3>& operator[](unsigned i) {
		return indices[i];
	}
private:
	string name;
	vector<array<unsigned, 3>> indices;
};
class Texture {
public:
	Texture(const Element* e) {
		const string file_name = e->getAttr("file_name");
		Framework::instance().createTexture(&texture, file_name.c_str());
		this->name = e->getAttr("name");
		GameLib::cout << "create texture: " << name.c_str() << GameLib::endl;
	}
	~Texture() {
		Framework::instance().destroyTexture(&texture);
		GameLib::cout << "delete texture: " << name.c_str() << GameLib::endl;
	}
	void set()const {
		Framework::instance().setTexture(texture);
	}
private:
	string name;
	GameLib::Texture* texture;
};
class Painter {
public:
	Painter(VertexBuffer* vb, IndexBuffer* ib, Texture* t, bool isZTest, GameLib::Framework::BlendMode mode) :vb_(vb), ib_(ib), texture_(t), isZTest_(isZTest), blend_mode_(mode) {
		calculate();
	}
	~Painter() {
		delete vb_;
		vb_ = nullptr;

		delete ib_;
		ib_ = nullptr;

		delete texture_;
		texture_ = nullptr;
	}
	const string& getName()const {
		return name;
	}
	void setName(const string& name) {
		this->name = name;
	}
	vector<Triangle> getTriangles()const {
		vector<Triangle> res;
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			res.push_back(Triangle(vb_->vertex(i0), vb_->vertex(i1), vb_->vertex(i2)));
		}
		return res;
	}
	void calculate() { // 初始化的时候计算法线贴图
		norms_.resize(vb_->size());
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			Vector3 norm = (vb_->vertex(i1) - vb_->vertex(i0)).cross(vb_->vertex(i2) - vb_->vertex(i0)).normalize();
			norms_[i0] += norm;
			norms_[i1] += norm;
			norms_[i2] += norm;
		}
		for (int i = 0; i < vb_->size(); i++) {
			norms_[i].normalize();
		}
	}
	void draw(const Matrix44& pv, const Matrix44& wm, const Light* light)const {
		vector<Vector3> res(vb_->size());
		vector<Vector3> world_coords(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			world_coords[i] = wm.vecMul(vb_->vertex(i));
		}
		for (int i = 0; i < vb_->size(); i++) {
			res[i] = pv.vecMul(world_coords[i]);
		}
		Framework f = Framework::instance();
		if (blend_mode_ == Framework::BLEND_OPAQUE) {
			f.enableDepthWrite(true);
		}
		else {
			f.enableDepthWrite(false);
		}
		if (texture_)
			texture_->set();
		else
			f.setTexture(nullptr); // TODO这里的封装不太优雅
		f.enableDepthTest(isZTest_);
		f.setBlendMode(blend_mode_);
		vector<unsigned> colors(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			Matrix44 wm_tmp = wm.dropRotation(); // 这里需要剔除掉法线向量平移的分量
			Vector3 transformed_norm = wm.vecMul(norms_[i]).normalize(); 
			colors[i] = light->calculate(transformed_norm,vb_->color(i));
		}
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			unsigned c0 = vb_->color(i0);
			unsigned c1 = vb_->color(i1);
			unsigned c2 = vb_->color(i2);
			Vector3 norm = (world_coords[i1] - world_coords[i0]).cross(world_coords[i2] - world_coords[i0]).normalize(); // 光照的计算使用世界坐标 
			
			// 注意这里不能用提前算好的法向量，因为wm矩阵有旋转成分
			c0=light->calculate(norm, c0);
			c1 = light->calculate(norm, c1);
			c2 = light->calculate(norm, c2);
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_->uv(i0).data(), vb_->uv(i1).data(), vb_->uv(i2).data(), c0,c1,c2);
		}
	}
private:
	string name; // 文件中的名称
	VertexBuffer* vb_;
	IndexBuffer* ib_;
	Texture* texture_;
	bool isZTest_;
	GameLib::Framework::BlendMode blend_mode_;
	vector<Vector3> norms_; // 法线贴图
};
// 实现物体之间的相对运动
class TransformNode {
public:
	TransformNode():TransformNode(nullptr) {}
	TransformNode(const Painter* painter) {
		painter_ = painter;
		scale_ = { 1,1,1 };
		animation_node_ = nullptr;
	}
	void setAnimationNode(AnimationNode *animation_node) {
		animation_node_ = animation_node;
	}
	const AnimationNode* getAnimationNode()const {
		return animation_node_;
	}
	void update(double time) {
		if (animation_node_==nullptr)
			return;
		animation_node_->update(translation_,rotation_,scale_,time);
	}

	void draw(const Matrix44&pv,const Matrix44& parent_m,Light*light){
		Matrix44 m;
		m.setTranslation(translation_);
		m.rotateY(rotation_.y);
		m.rotateX(rotation_.x);
		m.rotateZ(rotation_.z);
		m.scale(scale_);
		m = parent_m.matMul(m);
		if(painter_)
			painter_->draw(pv, m, light);
		for (int i = 0; i < children_.size(); i++)
			children_[i]->draw(pv, m, light);
	}
	vector<TransformNode*> getChildren()const {
		return children_;
	}
	TransformNode* getChild(int i) {
		return children_[i];
	}
	void setChild(int i, TransformNode* child) {
		children_.resize(i+1);
		children_[i]=child;
	}
	void setName(const string& name) {
		name_ = name;
	}
	const string& getName()const {
		return name_;
	}
	void setPainter(const Painter* painter) {
		painter_ = painter;
	}
	void setTranslation(const Vector3& v) {
		translation_ = v;
	}
	void setRotation(const Vector3& v) {
		rotation_ = v;
	}
	void setScale(const Vector3& v) {
		if (v.x == 0 && v.y == 0 && v.z == 0) {
			// 不能设置为0
			scale_ = { 1,1,1 };
			return;
		}
		scale_ = v;
	}
	TransformNode* addChild(TransformNode* child) {
		children_.push_back(child);
		return this;
	}
private:
	string name_;
	vector<TransformNode*> children_;
	Vector3 rotation_;
	Vector3 translation_;
	Vector3 scale_;
	const Painter* painter_;
	AnimationNode* animation_node_;
};