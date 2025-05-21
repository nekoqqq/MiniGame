#pragma once
#include <vector>
#include <array>
#include <cassert>
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Manager.h"
#include "Math.h"
#include "Xml.h"
#include "Collision.h"
#include "Model.h"

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
	Painter(VertexBuffer* vb, IndexBuffer* ib, Texture* t, bool isZTest, GameLib::Framework::BlendMode mode) :vb_(vb), ib_(ib), texture_(t), isZTest_(isZTest), blend_mode_(mode) {}
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
	vector<Triangle> getTriangles()const {
		vector<Triangle> res;
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			res.push_back(Triangle(vb_->vertex(i0), vb_->vertex(i1), vb_->vertex(i2)));
		}
		return res;
	}
	void draw(Matrix44& pvm) {
		vector<Vector3> res(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			res[i] = pvm.vecMul(vb_->vertex(i));
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
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_->uv(i0).data(), vb_->uv(i1).data(), vb_->uv(i2).data(), vb_->color(i0), vb_->color(i1), vb_->color(i2));
		}
	}
private:
	string name; // 文件中的名称
	VertexBuffer* vb_;
	IndexBuffer* ib_;
	Texture* texture_;
	bool isZTest_;
	GameLib::Framework::BlendMode blend_mode_;
};