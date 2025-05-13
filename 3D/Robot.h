#pragma once
#include"Math.h"
#include "GameLib/Framework.h"
using GameLib::Framework;

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
			pos_ = { 0,0,50 };
		}
		else if (type == ENEMY) {
			pos_ = { 0,0,-50 };
		}
		setBody();
		
		const double t[][4]={
			{ 2,0,0,pos_.x },
			{ 0,2, 0 ,pos_.y},
			{ 0,0,-2 ,pos_.z},
			{ 0,0,0,1 }
		};
		world_transform = t;
	}
	~Mecha(){}




	void update() {};
	void draw(Matrix44 &viewTransform, Matrix44 & projectionTransform) {
		Framework f = Framework::instance();
		Matrix44 pvm = projectionTransform.matMul(viewTransform).matMul(world_transform);
		Vector3 res[8];
		for (int i = 0; i < 8; i++) {
			res[i] = pvm.vecMul(body[i]);
		}
		int tri_idx[12][3] = {
			{0,1,2},
			{0,2,3},
			{4,5,6},
			{4,6,7},
			{0,4,7},
			{0,3,7},
			{1,5,6},
			{1,2,6},
			{2,6,7},
			{2,3,7},
			{1,4,5},
			{0,1,4}
		};

		f.enableDepthTest(true);
		f.setBlendMode(Framework::BLEND_LINEAR);

		for (int i = 0; i < 4; i++) {
			f.drawTriangle3DH(res[tri_idx[i][0]], res[tri_idx[i][1]], res[tri_idx[i][2]],0,0,0,0xffff0000,0xff00ff00,0xff0000ff);
		}
		for (int i = 4; i < 8; i++) {
			f.drawTriangle3DH(res[tri_idx[i][0]], res[tri_idx[i][1]], res[tri_idx[i][2]], 0, 0, 0, 0xff8f0000, 0xff008f00, 0xff00008f);
		}		
		for (int i = 8; i < 12; i++) {
			f.drawTriangle3DH(res[tri_idx[i][0]], res[tri_idx[i][1]], res[tri_idx[i][2]], 0, 0, 0, 0xff4f0000, 0xff004f00, 0xff00004f);
		}
	}



private:
	Vector3 pos_; // 中心点在世界坐标系中的位置
	Vector3 body[8]; // 8个顶点在模型坐标中的位置
	Matrix44 world_transform; // 世界变换矩阵
	
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
	Stage() {
		vectors = new Vector3[4]{
			{-50,0,-50},
			{-50,0,50},
			{50,0,50},
			{50,0,-50}
		};
		texture_ = nullptr;
		GameLib::Framework::instance().createTexture(&texture_, "stage.tga");
	};
	Stage(Vector3& p0, Vector3& p1, Vector3& p2, Vector3 &p3) {
		vectors = new Vector3[4]{
			p0,
			p1,
			p2,
			p3
		};
	}

	~Stage() { delete[]vectors; vectors = nullptr; 
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
		f.setTexture(texture_);
		f.setBlendMode(GameLib::Framework::BLEND_LINEAR);
		//f.drawTriangle3DH(res[0], res[1], res[2], uv[0], uv[1], uv[2], 0xffff0000, 0xff00ff00, 0xff0000ff);
		//f.drawTriangle3DH(res[0], res[3], res[2], uv[0], uv[3], uv[2],0xff800000, 0xff008000, 0xff000080);

		f.drawTriangle3DH(res[0], res[1], res[2]);
		f.drawTriangle3DH(res[0], res[3], res[2]);
	}
private:
	Vector3 *vectors;
	GameLib::Texture* texture_;
};