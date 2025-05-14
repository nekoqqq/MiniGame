#pragma once
#include <vector>
#include <array>
#include <cassert>
#include"Math.h"
#include "GameLib/Framework.h"
using GameLib::Framework;
using std::vector;
using std::array;
namespace GameLib {
	class Texture;
}

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
	void move(double dx,double dy, double dz, Vector3& eye_pos, Vector3& target_pos, Vector3& up) {
		// 这里的移动是在相机坐标系内移动
		// 移动前坐标为Y，世界坐标X = AY+C，
		// 移动后坐标为Y',世界坐标X' = AY'+C,
		// 两式相减X'-X = A(Y'-Y) => X'=X+A(Y'-Y) => X'=X+A delta，其中delta是相机坐标系中的位移量
		// 或者 X=AY+C => X=A(Y+delta)+c => X=AY + c +A delta，增加量还是旋转*delta
		Matrix44 m = Matrix44::getViewRotation(eye_pos, target_pos, up);
		pos_ += m.vecMul({ dx,dy,dz });
	}

	const Vector3& getPos() {
		return pos_;
	}

	void update() {
		if (!type == PLAYER)
			return;
	};
	void draw(Matrix44 &viewTransform, Matrix44 & projectionTransform) {
		Framework f = Framework::instance();
		const double t[][4] = {
			{ 2,0,0,pos_.x },
			{ 0,2, 0 ,pos_.y},
			{ 0,0,-2 ,pos_.z},
			{ 0,0,0,1 }
		};

		// 世界变换矩阵
		Matrix44 world_transform = t;
		Matrix44 pvm = projectionTransform.matMul(viewTransform).matMul(world_transform);
		Vector3 res[8];
		for (int i = 0; i < 8; i++) {
			res[i] = pvm.vecMul(body[i]);
		}
		int tri_idx[12][3] = {
			// -y
			{0,1,2},
			{0,2,3},

			// +y
			{4,5,6},
			{4,6,7},

			// -z
			{0,4,7},
			{0,3,7},

			// +z
			{1,5,6},
			{1,2,6},

			//  +x
			{2,6,7},
			{2,3,7},

			// -x
			{1,4,5},
			{0,1,4}
		};

		
		int uv_idx[12][3] = {
			{6,7,12 },
			{6,12,11},
			{5,6,11},
			{5,11,10},



			{7,2,1},
			{7,6,1},
			{6,1,0},
			{6,5,0},



			{9,4,3},
			{9,8,3},
			{8,2,3},
			{7,8,2}
		};

		// 六个图形，一共5+5+3 = 13个顶点
		double uv[13][2] = {
			{0.0,0.0}, // 0
			{0.25,0.0}, // 1
			{0.5,0.0}, // 2
			{0.75,0.0}, // 3
			{1.0,0.0}, // 4
			{0.0,0.25},// 5
			{0.25,0.25}, // 6
			{0.5,0.25},// 7
			{0.75,0.25}, // 8
			{1.0,0.25}, // 9
			{0,0.375}, // 10
			{0.25,0.375}, // 11
			{0.5,0.375} // 12
		};

		f.enableDepthTest(true);
		f.setTexture(texture_);
		f.setBlendMode(Framework::BLEND_OPAQUE);

		for (int i = 0; i < 4; i++) {
			f.drawTriangle3DH(res[tri_idx[i][0]], res[tri_idx[i][1]], res[tri_idx[i][2]],uv[uv_idx[i][0]], uv[uv_idx[i][1]], uv[uv_idx[i][2]], 0xffff0000, 0xff00ff00, 0xff0000ff);
		}
		for (int i = 4; i < 8; i++) {
			f.drawTriangle3DH(res[tri_idx[i][0]], res[tri_idx[i][1]], res[tri_idx[i][2]], uv[uv_idx[i][0]], uv[uv_idx[i][1]], uv[uv_idx[i][2]], 0xff8f0000, 0xff008f00, 0xff00008f);
		}		
		for (int i = 8; i < 12; i++) {
			f.drawTriangle3DH(res[tri_idx[i][0]], res[tri_idx[i][1]], res[tri_idx[i][2]], uv[uv_idx[i][0]], uv[uv_idx[i][1]], uv[uv_idx[i][2]], 0xff4f0000, 0xff004f00, 0xff00004f);
		}
	}

	Vector3 getOrigin() {
		return pos_;
	}

	Matrix44 getRotationMatrix() {
		const double t[][4] = {
			{ 1,0,0 ,0},
			{ 0,1,0,0},
			{ 0,0,-1,0},
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
	Vector3 body[8]; // 8个顶点在模型坐标中的位置
	GameLib::Texture* texture_;

	void setBody() {
		body[0] = { -1,0,-1 };
		body[1] = { -1,0,1 };
		body[2] = { 1,0,1 };
		body[3] = { 1,0,-1 };
		body[4] = { -1,2,-1 };
		body[5] = { -1,2,1 };
		body[6] = { 1,2,1 };
		body[7] = { 1,2,-1 };
	}
};

class Stage
{
public:
	Stage(){
		vectors = new Vector3[4]{
			{-100,0,-100},
			{-100,0,100},
			{100,0,100},
			{100,0,-100}
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
	void draw(Matrix44&viewTransform, Matrix44 & projectionTransform)const
	{
		GameLib::Framework f = GameLib::Framework::instance();
		Vector3 res[4];
		for (int i = 0; i < 4; i++) {
			res[i] =  projectionTransform.vecMul(viewTransform.vecMul(vectors[i]));
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

		//f.drawTriangle3DH(res[0], res[1], res[2]);
		//f.drawTriangle3DH(res[0], res[3], res[2]);
	}
private:
	Vector3 *vectors;
	GameLib::Texture* texture_;
};

// 绘制辅助坐标轴
class Axis {
public:
	static void draw(Matrix44& viewTransform, Matrix44& projectionTransform) {
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
			res[i] = projectionTransform.vecMul(viewTransform.vecMul(x_axis[i]));
		}
		for (int i = 0; i < 4; i++) {
			res[4+i] = projectionTransform.vecMul(viewTransform.vecMul(y_axis[i]));
		}
		for (int i = 0; i < 4; i++) {
			res[8 + i] = projectionTransform.vecMul(viewTransform.vecMul(z_axis[i]));
		}
		Framework f = Framework::instance();
		f.enableDepthTest(true);
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
			dir_res[i] = projectionTransform.vecMul(viewTransform.vecMul(x_dir[i]));
		}
		for (int i = 0; i < 3; i++) {
			dir_res[3+i] = projectionTransform.vecMul(viewTransform.vecMul(y_dir[i]));
		}
		for (int i = 0; i < 3; i++) {
			dir_res[6+i] = projectionTransform.vecMul(viewTransform.vecMul(z_dir[i]));
		}
		f.drawTriangle3DH(dir_res[0], dir_res[1], dir_res[2], 0, 0, 0, 0xffff0000, 0xffff0000, 0xffff0000);
		f.drawTriangle3DH(dir_res[3 + 0], dir_res[3 + 1], dir_res[3 + 2], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00);
		f.drawTriangle3DH(dir_res[6 + 0], dir_res[6 + 1], dir_res[6 + 2], 0, 0, 0, 0xff0000ff, 0xff0000ff, 0xff0000ff);
	}
};

class VertexBuffer {
public:
	VertexBuffer(){}
	VertexBuffer(int n)
	{
		points_.resize(n);
	}
	~VertexBuffer()
	{
	}
	const Vector3& operator[](int i)const {
		return points_[i];
	}
	Vector3& operator[](int i) {
		return points_[i];
	}
	void setPoint(int i, Vector3& v) {
		assert(0 <= i && i < points_.size());
		points_[i] = v;
	}
private:
	vector<Vector3> points_;
	vector<array<int,2>> uvs_;
};

class IndexBuffer {
public:
	IndexBuffer() {}
	IndexBuffer(int n) {
		indexes_.resize(n);
	}
	~IndexBuffer(){}
	void setIndex(int i,int value) {
		assert(0 <= i && i < indexes_.size());
		indexes_[i] = value;
	}
	int size()const {
		return indexes_.size();
	}
private:
	vector<int> indexes_;
};