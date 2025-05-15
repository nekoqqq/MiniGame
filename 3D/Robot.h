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

class VertexBuffer {
public:
	VertexBuffer() {}
	VertexBuffer(int n) // 存了多少点的坐标
	{
		points_.resize(n);
		uvs_.resize(n);
	}
	VertexBuffer(const vector<Vector3>& vertexes) {
		points_ = vertexes;
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

private:
	vector<Vector3> points_;
	vector<array<double, 2>> uvs_;
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

class Model {
public:
	enum Type {
		STAGE,
		PLAYER,
		ENEMY
	};
	Model() {}
	~Model() {}
	void draw(Matrix44& world_transform, Matrix44& viewTransform, Matrix44& projectionTransform) {
		Matrix44 composite_matrix = projectionTransform.matMul(viewTransform.matMul(world_transform));

		vector<Vector3> res(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			res[i] = composite_matrix.vecMul(vb_->vertex(i));
		}

		Framework f = Framework::instance();
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_->uv(i0).data(), vb_->uv(i1).data(), vb_->uv(i2).data());
		}
	}
private:
	Type type_;
	Vector3 pos_;

	VertexBuffer* vb_;
	IndexBuffer* ib_;
	GameLib::Texture* texture_;
};

class Camera {
public:
	Camera(const Vector3 &eye_pos,const Vector3 &target_pos, const Vector3& up,double fov_y,double near,double far, double aspect_ratio):eye_pos_(eye_pos),target_pos_(target_pos),up_(up),fov_y(fov_y),near(near),far(far),aspect_ratio(aspect_ratio){
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
	void update(const Vector3 &player_dir) {
		eye_pos_ = player_dir + Vector3(0, 10, -5);
		target_pos_ = player_dir + Vector3(0, 0, 10);
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
		double (& p)[4][4] = projectionTransform.p;
		p[1][1] = 1 / tan(fov_y * 0.5);
		p[0][0] = p[1][1] / aspect_ratio;
		p[2][2] = far / (far - near);
		p[2][3] = -near * far / (far - near);
		p[3][2] = 1;
	}

};

class Mecha
{
public:
	enum Type
	{
		PLAYER,
		ENEMY
	};

	Mecha(Type type){
		if (type == PLAYER) {
			pos_ = { 0,0,-50 };
			Framework::instance().createTexture(&texture_, "player.tga");
		}
		else if (type == ENEMY) {
			pos_ = { 0,0,50 };
		}
		setBody();
		this->type = type;
	}
	~Mecha(){
		Framework::instance().destroyTexture(&texture_);
	}
	void move( Matrix44 &vr) {
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
		pos_ += vr.vecMul({ dx,dy,dz });
	}
	const Vector3& getPos() {
		return pos_;
	}
	void update() {
		if (!type == PLAYER)
			return;
	};
	void draw(const Matrix44 & pv) {
		// 先做点的变换
		Matrix44 world_transform = getWorldMatrix();
		Matrix44 pvm = pv.matMul(world_transform);
		vector<Vector3> res;
		for (int i = 0; i < vb_.size(); i++)
			res.push_back(pvm.vecMul(vb_.vertex(i)));

		// 绘制三角形
		Framework f = Framework::instance();
		f.enableDepthTest(true);
		f.setTexture(texture_);
		f.setBlendMode(Framework::BLEND_LINEAR);
		int n = ib_.size();
		for (int i = 0; i < n; i++) {
			int i0 = ib_[i][0];
			int i1 = ib_[i][1];
			int i2 = ib_[i][2];
			int c0 = 0;
			int c1 = 0;
			int c2 = 0;
			if (i/8 == 0) {
				c0 = 0xffff0000;
				c1 = 0xff00ff00;
				c2= 0xff0000ff;
			}
			else if (i/8 == 1) {
				c0 = 0xff8f0000;
				c1 = 0xff008f00;
				c2 = 0xff00008f;
			}
			else if (i/8 ==2) {
				c0 = 0xff4f0000;
				c1 = 0xff004f00;
				c2 = 0xff00004f;
			}
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_.uv(i0).data(), vb_.uv(i1).data(), vb_.uv(i2).data(),c0,c1,c2);
			//f.drawTriangle3DH(res[i0], res[i1], res[i2], 0,0,0, c0, c1, c2);
		}
	}
	Vector3 getOrigin() {
		return pos_;
	}
	Matrix44 getRotationMatrix() {
		const double t[][4] = {
			{ 1,0,0 ,0},
			{ 0,1,0,0},
			{ 0,0,1,0},
			{ 0,0,0,1}
		};
		Matrix44 m(t);
		return m;
	}
	Matrix44 getWorldMatrix() {
		Matrix44 r = getRotationMatrix();
		r[0][3] = pos_.x;
		r[1][3] = pos_.y;
		r[2][3] = pos_.z;
		return r;
	}

	Vector3 getWorldDirection() {
		Vector3 v(0, 0, 1);
		Matrix44 world_transform = getWorldMatrix();
		return world_transform.vecMul(v);
	}
private:
	Type type;
	Vector3 pos_; // 中心点在世界坐标系中的位置
	GameLib::Texture* texture_;
	VertexBuffer vb_;
	IndexBuffer ib_;

	void setBody() {
		vector<Vector3> vertexes = {
			{1,1,1},  // +z
			{-1,1,1},
			{-1,0,1},
			{1,0,1},

			{-1,1,-1}, // -z
			{1,1,-1},
			{1,0,-1},
			{-1,0,-1},

			{ -1,1,1 }, // -x
			{-1,1,-1},
			{-1,0,-1},
			{-1,0,1},

			{1,1,-1}, // +x
			{1,1,1},
			{1,0,1},
			{1,0,-1},

			{-1,1,1}, // +y
			{1,1,1},
			{1,1,-1},
			{-1,1,-1},

			{1,0,1}, // -y
			{-1,0,1},
			{-1,0,-1},
			{1,0,-1}
		};
		vector<array<double, 2>> uvs = {
			// +z
			{0.0,0.0},
			{0.25,0.0},
			{0.25,0.25},
			{0.0,0.25},

			// -z
			{0.25,0.0},
			{0.5,0.0},
			{0.5,0.25},
			{0.25,0.25},

			// -x
			{0.5,0.0},
			{0.75,0.0},
			{0.75,0.25},
			{0.5,0.25},

			// +x
			{0.75,0.0},
			{1.0,0.0},
			{1.0,0.25},
			{0.75,0.25},

			// +y
			{0.25,0.0},
			{0.25,0.25},
			{0.25,0.375},
			{0.0,0.375},

			// -y
			{0.25,0.25},
			{0.5,0.25},
			{0.5,0.375},
			{0.25,0.375}
		};
		vb_.setInternal(vertexes, uvs);
		vector<array<unsigned, 3>> indices = {
			{0,1,2},
			{3,2,0},

			{4,5,6},
			{7,6,4},

			{8,9,10},
			{11,10,8},

			{12,13,14},
			{15,14,12},

			{16,17,18},
			{19,18,16},

			{20,21,22},
			{23,22,20}
		};
		ib_ = indices;
	}
};

class Stage
{
public:
	Stage(){
		vectors = new Vector3[4]{
			{-1000,0,-1000},
			{-1000,0,1000},
			{1000,0,1000},
			{1000,0,-1000}
		};
		init();
	};
	Stage(Vector3& p0, Vector3& p1, Vector3& p2, Vector3 &p3) {
		vectors = new Vector3[4]{
			p0,
			p1,
			p2,
			p3
		};
		init();
	}
	void init() {
		GameLib::Framework::instance().createTexture(&texture_, "stage.tga");
	}
	~Stage() { 
		delete[]vectors; 
		vectors = nullptr; 
		GameLib::Framework::instance().destroyTexture(&texture_);
		texture_ = nullptr; 
	};
	void draw(const Matrix44 &pv)const
	{
		GameLib::Framework f = GameLib::Framework::instance();
		Vector3 res[4];
		for (int i = 0; i < 4; i++) {
			res[i] = pv.vecMul(vectors[i]);
		}
		double uv[][2] = {
			{0,0},
			{1,0},
			{1,1},
			{0,1}
		};

		// 这个API不要求x、y、的范围，但是只有[-1,1] x [-1,1] x [0,1]的图形会被绘制
		f.enableDepthTest(true);
		f.setTexture(texture_);
		f.setBlendMode(GameLib::Framework::BLEND_LINEAR);
		f.drawTriangle3DH(res[0], res[1], res[2], uv[0], uv[1], uv[2]);
		f.drawTriangle3DH(res[0], res[3], res[2], uv[0], uv[3], uv[2]);
	}
private:
	Vector3 *vectors;
	GameLib::Texture* texture_;
};

// 绘制辅助坐标轴
class Axis {
public:
	static void draw(const Matrix44 &pvm) {
		// 绘制坐标轴		
		Vector3 x_axis[4] = {
			{-100,1,0,1},
			{100,1,0,1},
			{100,-1,0,1},
			{-100,-1,0,1},
		};
		Vector3 y_axis[4] = {
			{-1,-100,0,1},
			{1,-100,0,1},
			{1,100,0,1},
			{-1,100,0,1},
		};
		Vector3 z_axis[4] = {
			{-1,0,100,1},
			{1,0,100,1},
			{1,0,-100,1},
			{-1,0,-100,1}
		};
		Vector3 res[12];
		for (int i = 0; i < 4; i++) {
			res[i] = pvm.vecMul(x_axis[i]);
		}
		for (int i = 0; i < 4; i++) {
			res[4+i] = pvm.vecMul(y_axis[i]);
		}
		for (int i = 0; i < 4; i++) {
			res[8 + i] = pvm.vecMul(z_axis[i]);
		}
		Framework f = Framework::instance();
		f.enableDepthTest(true);
		f.setBlendMode(Framework::BLEND_LINEAR);
		f.drawTriangle3DH(res[0], res[1], res[2], 0, 0, 0, 0xffff0000, 0xffff0000, 0xffff0000);
		f.drawTriangle3DH(res[0], res[2], res[3], 0, 0, 0, 0xffff0000, 0xffff0000, 0xffff0000);
		f.drawTriangle3DH(res[4+0], res[4+1], res[4+2], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00);
		f.drawTriangle3DH(res[4+0], res[4+2], res[4+3], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00);
		f.drawTriangle3DH(res[8 + 0], res[8 + 1], res[8 + 2], 0, 0, 0, 0xff0000ff, 0xff0000ff, 0xff0000ff);
		f.drawTriangle3DH(res[8 + 0], res[8 + 2], res[8 + 3], 0, 0, 0, 0xff0000ff, 0xff0000ff, 0xff0000ff);


		// 绘制箭头
		Vector3 x_dir[] = {
			{100,5,0,1},
			{120,0,0,1},
			{100,-5,0,1}
		};
		Vector3 y_dir[] = {
			{-5,100,0,1},
			{5,100,0,1},
			{0,120,0,1}
		};
		Vector3 z_dir[] = {
			{0,5,100,1},
			{0,-5,100,1},
			{0,0,120,1}
		};
		Vector3 dir_res[9];
		for (int i = 0; i < 3; i++) {
			dir_res[i] = pvm.vecMul(x_dir[i]);
		}
		for (int i = 0; i < 3; i++) {
			dir_res[3+i] = pvm.vecMul(y_dir[i]);
		}
		for (int i = 0; i < 3; i++) {
			dir_res[6+i] = pvm.vecMul(z_dir[i]);
		}

		f.enableDepthTest(true);
		f.setBlendMode(Framework::BLEND_LINEAR);
		f.drawTriangle3DH(dir_res[0], dir_res[1], dir_res[2], 0, 0, 0, 0xffff0000, 0xffff0000, 0xffff0000);
		f.drawTriangle3DH(dir_res[3 + 0], dir_res[3 + 1], dir_res[3 + 2], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00);
		f.drawTriangle3DH(dir_res[6 + 0], dir_res[6 + 1], dir_res[6 + 2], 0, 0, 0, 0xff0000ff, 0xff0000ff, 0xff0000ff);
	}
};

