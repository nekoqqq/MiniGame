#pragma once
#include <vector>
#include <array>
#include <cassert>
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "Math.h"
#include "Xml.h"
#include "Collision.h"
#include "Graph.h"


using std::ostringstream;
using namespace GameLib::Input;
using GameLib::Framework;
using std::vector;
using std::array;
using std::max;
using std::min;
namespace GameLib {
	class Texture;
}
extern int gCounter;
extern const int FRAMES;
extern const double PI;
const int JUMP_UP_DURATION = 0.4*FRAMES;
const int SKY_STAY = 0.2*FRAMES; // 滞空时间
const int FALL_DURATION = 0.4 * FRAMES;

const int MAX_MISSLES = 3;
const int MISSLE_TTL = 3*FRAMES; // 5秒

// 转身
const double ZOOM_DURATION = (JUMP_UP_DURATION+SKY_STAY+FALL_DURATION)/3; // 持续多少帧
const double TURN_DEGREE = 180.0 / FRAMES; // 每帧视角转换角度

// 导弹
const double MISSLE_ROTATION_SPEED = 360.0 / FRAMES; // 度/每帧
const int MAX_HP = 1000;
const int MAX_ENEGY = 100;
const int MISSLE_ENGEY_COST = 25; // 每个导弹消耗的能量值

// 人物
const int ENEGY_RECOVER = 180/ FRAMES; // 每秒恢复的能量
const int MAX_LOCK_ON = 10; // 锁定人物的角度范围
const int MIN_LOCK_OFF = 20; // 人物跳出范围的角度,也就是说锁定后在0-60的范围内都算作锁定

inline Vector3 getCuboidHalf() {
	return { 1.0,1.0,1.0 };
}
class Model {
public:
	enum Type {
		UNKNOW,
		STAGE,
		PLAYER,
		ENEMY,
		AXIS,
		MISSLE
	};
	Model(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m) :type_(type), pos_(pos), painter_(painter),collision_type_(collision_type), world_transform_(m) {
		world_transform_ = m;
		world_transform_[0][3] = pos.x;
		world_transform_[1][3] = pos.y;
		world_transform_[2][3] = pos.z;
		world_transform_[3][3] = 1;
		collision_model_ = nullptr;
	}
	virtual ~Model() {
		test_collision_models.clear();
	}
	virtual void draw(const Matrix44 &pv, const Light* light)=0 {
		painter_->draw(pv, getModelTransform(), light);
	}
	virtual void update(const Matrix44& vr) = 0;
	void initCollisionModel(const Vector3&cuboid_origin,const Vector3 & half,const Vector3& sphere_origin,double r) {
		if (collision_type_ == CollisionModel::Type::CUBOID) {
			collision_model_ = new Cuboid(cuboid_origin,half);
		}
		else if (collision_type_ == CollisionModel::Type::SPHERE) {
			collision_model_ = new Sphere(sphere_origin, r);
		}
		else if (collision_type_ == CollisionModel::Type::TRIANGLE) {
			collision_model_ = new Triangle();
		}
	}
	void updateCollisionPos(const Vector3 & center) {
		if (collision_type_ == CollisionModel::Type::CUBOID) {
			dynamic_cast<Cuboid*>(collision_model_)->center =center;
		}
		else if (collision_type_ == CollisionModel::Type::SPHERE) {
			dynamic_cast<Sphere*>(collision_model_)->center = center;
		}
	}
	void addCollisionModel(Model* collision_model) {
		test_collision_models.push_back(collision_model);
	}
	void setCollisionModels(const vector<Model*>& collision_model) {
		test_collision_models = collision_model;
	}
	const vector<Model*>& getCollisionModels()const {
		return test_collision_models;
	}

	bool isCollision(const Model* other) {
		return collision_model_->isCollision(*other->collision_model_);
	}
	const Vector3& getPos() const {
		return pos_;
	}
	Vector3 getZDirection() {
		Vector3 v(0, 0, 1);
		Matrix44 world_transform = getModelRotation();
		return world_transform.vecMul(v);
	}
	const CollisionModel* getCollsionModel()const {
		return collision_model_;
	}
	const Matrix44& getModelTransform() { // 这里有些trick，会在get的时候设置这个值
		world_transform_[0][3] = pos_.x;
		world_transform_[1][3] = pos_.y;
		world_transform_[2][3] = pos_.z;
		return world_transform_;
	}

protected:
	Matrix44 getModelRotation()const{
		Matrix44 world_rotation = world_transform_;
		world_rotation[0][3] = world_rotation[1][3] = world_rotation[2][3] = 0;
		world_rotation[3][3] = 1.0;
		return world_rotation;
	}
	void setRotationZ(double theta) {
		double t = theta * PI / 180.0;
		world_transform_[0][0] = cos(t); world_transform_[0][1] = -sin(t); world_transform_[0][2] = 0.0; world_transform_[0][3] = 0.0;
		world_transform_[1][0] = sin(t); world_transform_[1][1] = cos(t); world_transform_[1][2] = 0.0; world_transform_[1][3] = 0.0;
		world_transform_[2][0] = 0.0; world_transform_[2][1] = 0.0; world_transform_[2][2] = 1.0; world_transform_[2][3] = 0.0;
		world_transform_[3][0] = 0.0; world_transform_[3][1] = 0.0; world_transform_[3][2] = 0.0; world_transform_[3][3] = 1.0;
	}
	void setRotationY(double theta) { // 设置世界变换的旋转矩阵为绕Y轴转多少度，相当于初始化的操作
		double t = theta * PI / 180.0;
		world_transform_[0][0] = cos(t); world_transform_[0][1] = 0.0; world_transform_[0][2] = sin(t); world_transform_[0][3] = 0.0;
		world_transform_[1][0] = 0.0; world_transform_[1][1] = 1.0; world_transform_[1][2] = 0.0; world_transform_[1][3] = 0.0;
		world_transform_[2][0] = -sin(t); world_transform_[2][1] = 0.0; world_transform_[2][2] = cos(t); world_transform_[2][3] = 0.0;
		world_transform_[3][0] = 0.0;world_transform_[3][1] = 0.0;world_transform_[3][2] = 0.0;world_transform_[3][3] = 1.0;
	}
	void rotateX(double theta) {
		const Vector3& dir{ 1,0,0 };
		rotateDirection(dir, theta);
	}
	void rotateY(double theta) {
		const Vector3& dir{ 0,1,0 };
		rotateDirection(dir, theta);
	}
	void rotateZ(double theta) {
		const Vector3& dir{ 0,0,1 };
		rotateDirection(dir, theta);
	}
	void rotateDirection(const Vector3& direction,double theta) { // 绕着自身的任意方向，用左手，大拇指和轴的正方向一致，旋转theta度
		double t = theta * PI / 180.0;
		const Vector3 unit_dir = direction *1.0/ direction.norm(); // 这里需要检查一下是否为0
		Matrix44 id = Matrix44::identity();
		Matrix44 a = id * cos(t);
		Matrix44 b = Matrix44::getOuterMatrix(unit_dir) * (1 - cos(t));
		Matrix44 c = Matrix44::getCrossMatrix(unit_dir) * sin(t);
		Matrix44 res= a+b+c;
		res[3][3] = 1.0;
		world_transform_ = getModelTransform().matMul(res); // 这里的trick在于先get，从而更新世界矩阵
	}

	void setPos(double x, double y, double z) {
		setPos({ x,y,z });
	}
	void setPos(const Vector3& v) {
		pos_ = v;
	}
	Type getType()const
	{
		return type_;
	}

private:
	Type type_;
	Vector3 pos_;
	Painter* painter_;
	Matrix44 world_transform_;
	CollisionModel *collision_model_;
	CollisionModel::Type collision_type_;
	vector<Model*> test_collision_models; // 会发生碰撞的其他物体
};
class Stage :public Model
{
public:
	Stage(Type type, Painter* painter, CollisionModel::Type collision_type) :Model(type, { 0.0,0.0,0.0 }, painter, collision_type, Matrix44::identity()) {
		triangles_ = painter->getTriangles();
		double r = getHalf();
		initCollisionModel({ 0.0,0.0,0.0 }, { 1000.0, 0.0, 1000.0 }, { 0.0,-r,0.0 }, r);
	};
	~Stage() {
	};
	const vector<Triangle>& getTriangles()const {
		return triangles_;
	}
	double getHalf()const
	{
		return 5000.0;
	}
	virtual void update(const Matrix44& vr)override {}
	virtual void draw(const Matrix44& pvm, const Light* light)override {
		Model::draw(pvm, light);
	}
private:
	vector<Triangle> triangles_;
};


// 绘制辅助坐标轴
class Axis :public Model {
public:
	Axis(Type type, Painter* painter, CollisionModel::Type collision_type) :Model(type, { 0.0,0.0,0.0 }, painter, collision_type, Matrix44::identity()) {}
	~Axis() {}
	virtual void update(const Matrix44& pvm) override {}
	virtual void draw(const Matrix44& pv, const Light* light)override {
		Model::draw(pv, light);
	}
};