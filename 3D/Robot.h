#pragma once
#include <vector>
#include <array>
#include <cassert>
#include"Math.h"
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Manager.h"
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

class Camera {
public:
	Camera(const Vector3& eye_pos, const Vector3& target_pos, const Vector3& up, double fov_y, double near, double far, double aspect_ratio) :eye_pos_(eye_pos), target_pos_(target_pos), up_(up), fov_y(fov_y), near(near), far(far), aspect_ratio(aspect_ratio) {
		setProjectionTransform();
	}
	~Camera() {}
	Matrix44 getViewRotation() {
		Vector3 e3 = (target_pos_ - eye_pos_).normalize();
		Vector3 e1 = up_.cross(e3).normalize();
		Vector3 e2 = e3.cross(e1).normalize();
		const double rot_t[][4] = {
			{e1.x,e2.x,e3.x,0},
			{e1.y,e2.y,e3.y,0},
			{e1.z,e2.z,e3.z,0},
			{0,0,0,1}
		};

		return Matrix44(rot_t);
	}

	Matrix44 getViewProjectionMatrix() {
		Matrix44 rotation = getViewRotation().transpose();
		const double trans_t[][4] = {
			{1.,0.,0.,-eye_pos_.x },
			{0.,1.,0.,-eye_pos_.y },
			{ 0.,0.,1.,-eye_pos_.z},
			{0,0,0,1}
		};
		Matrix44 trans(trans_t);
		return projectionTransform.matMul(rotation.matMul(trans));
	}
	void update(const Vector3& player_dir) {
		eye_pos_ = player_dir + Vector3(0, 10, -5);
		target_pos_ = player_dir + Vector3(0, 0, 5);
	}
private:
	Vector3 eye_pos_;
	Vector3 target_pos_;
	Vector3 up_;
	double fov_y;
	double near;
	double far;
	double aspect_ratio;
	Matrix44 projectionTransform;

	void setProjectionTransform() {
		double(&p)[4][4] = projectionTransform.p;
		p[1][1] = 1 / tan(fov_y * 0.5);
		p[0][0] = p[1][1] / aspect_ratio;
		p[2][2] = far / (far - near);
		p[2][3] = -near * far / (far - near);
		p[3][2] = 1;
	}
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
	~VertexBuffer()
	{
	}
	int size()const {
		return points_.size();
	}
	void setInternal(const vector<Vector3>& vertexes,const vector<array<double, 2>>& uv){
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
	~IndexBuffer() {}
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
	vector<array<unsigned, 3>> indices;
};

class Painter {
public:
	Painter(VertexBuffer*vb,IndexBuffer *ib,GameLib::Texture* t,bool isZTest,GameLib::Framework::BlendMode mode):vb_(vb),ib_(ib),texture_(t),isZTest_(isZTest),blend_mode_(mode){}
	~Painter() {
		delete vb_;
		vb_ = nullptr;

		delete ib_;
		ib_ = nullptr;

		delete texture_;
		texture_ = nullptr;
	}
	void draw(Matrix44 &pvm){
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
		f.setTexture(texture_);
		f.enableDepthTest(isZTest_);
		f.setBlendMode(blend_mode_);
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_->uv(i0).data(), vb_->uv(i1).data(), vb_->uv(i2).data(),vb_->color(i0), vb_->color(i1), vb_->color(i2));
		}
	}
private:
	VertexBuffer* vb_;
	IndexBuffer* ib_;
	GameLib::Texture* texture_;
	bool isZTest_;
	GameLib::Framework::BlendMode blend_mode_;
};

class Model {
public:
	enum Type {
		UNKNOW,
		STAGE,
		PLAYER,
		ENEMY,
		AXIS
	};
	Model(Type type,const Vector3 &pos, Painter *painter, const Matrix44 &m ):type_(UNKNOW),pos_(pos), painter_(painter),world_rotation_(m) {}
	virtual ~Model() {
		delete painter_;
		painter_ = nullptr;
	}
	void draw(const Matrix44 &pv){
		Matrix44 model_transform = getModelTransform();
		Matrix44 pvm = pv.matMul(model_transform);
		painter_->draw(pvm);
	}
	virtual void update(const Matrix44& vr) = 0;
	Vector3 getZDirection() {
		Vector3 v(0, 0, 1);
		Matrix44 world_transform = getModelTransform();
		return world_transform.vecMul(v);
	}
protected:
	const Matrix44& getModelRotation()const{
		return world_rotation_;
	}
	Vector3& getPos() {
		return pos_;
	}
	void setPos(double x, double y, double z) {
		setPos({ x,y,z });
	}
	void setPos(const Vector3 &v) {
		pos_ = v;
	}
	Matrix44 getModelTransform() const{
		Matrix44 r = getModelRotation();
		r[0][3] = pos_.x;
		r[1][3] = pos_.y;
		r[2][3] = pos_.z;
		return r;
	}
private:
	Type type_;
	Vector3 pos_;
	Painter* painter_;
	Matrix44 world_rotation_;
};


class Mecha:public Model{
public:
	Mecha(Type type, const Vector3& pos, Painter *painter,const Matrix44 &m = Matrix44::identity()):Model(type,pos,painter,m){
	}
	~Mecha(){
	}
	virtual void update(const Matrix44& vr)override {
		Keyboard k = Manager::instance().keyboard();
		double dx = 0., dy = 0., dz = 0.;
		if (k.isOn('w')) {
			dz = 1.0;
		}
		if (k.isOn('a')) {
			dx = -1.0;
		}
		if (k.isOn('s')) {
			dz = -1.0;
		}
		if (k.isOn('d')) {
			dx = 1.0;
		}
		if (k.isOn(' ')) {
			dy = 1.0;
		}
		if (k.isOn('z')) {
			dy = -1.0;
		}
		// 这里的移动是在相机坐标系内移动
		// 移动前坐标为Y，世界坐标X = AY+C，
		// 移动后坐标为Y',世界坐标X' = AY'+C,
		// 两式相减X'-X = A(Y'-Y) => X'=X+A(Y'-Y) => X'=X+A delta，其中delta是相机坐标系中的位移量
		// 或者 X=AY+C => X=A(Y+delta)+c => X=AY + c +A delta，增加量还是旋转*delta
		setPos(getPos() + vr.vecMul({ dx,dy,dz }));
	};
};

class Stage:public Model
{
public:
	Stage(Type type,Painter * painter) :Model(type, { 0,0,0 },painter,Matrix44::identity()) {
	};
	~Stage() { 
	};
	virtual void update(const Matrix44& vr)override{}
};

// 绘制辅助坐标轴
class Axis:public Model {
public:
	Axis(Type type,Painter * painter) :Model(type, { 0,0,0 },painter,Matrix44::identity()){}
	~Axis(){}
	virtual void update(const Matrix44& pvm) override {}
};

