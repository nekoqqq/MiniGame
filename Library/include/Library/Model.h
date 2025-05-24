#pragma once
#include <sstream>
#include <vector>
#include <list>
#include <cmath>
#include <array>
#include <cassert>
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Manager.h"
#include "Math.h"
#include "Xml.h"
#include "Collision.h"
#include "Graph.h"
#include "Globals.h"

using std::ostringstream;
using namespace GameLib::Input;
using GameLib::Framework;
using std::vector;
using std::array;
namespace GameLib {
	class Texture;
}
extern const int FRAMES;
extern const double PI;
const int MAX_ENEGY = 1*FRAMES;
const int SKY_STAY = 0.5*FRAMES; // 滞空时间
const int FALL_DURATION = 0.5 * FRAMES;

const int MAX_MISSLES = 3;
const int MISSLE_TTL = 5*FRAMES; // 5秒

// 移动速度
const double MAX_SPEED = 4.0;
const double ACC_DURATION = 2.0; // 单位秒
double FRAME_SPEED_ACC = MAX_SPEED / (ACC_DURATION * FRAMES);

// 转身
const double TURN_DURATION = (MAX_ENEGY+SKY_STAY+FALL_DURATION)/3; // 持续多少帧

// 导弹
const double MISSLE_ROTATION_SPEED = 360.0 / FRAMES; // 度/每帧
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
	}
	virtual void draw(const Matrix44 &pv)=0 {
		Matrix44 model_transform = getModelTransform();
		Matrix44 pvm = pv.matMul(model_transform);
		painter_->draw(pvm);
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
	Matrix44 getModelTransform() { // 这里有些trick，会在get的时候设置这个值
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
		Matrix44 b = getOuterMatrix(unit_dir) * (1 - cos(t));
		Matrix44 c = getCrossMatrix(unit_dir) * sin(t);
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

private:
	Type type_;
	Vector3 pos_;
	Painter* painter_;
	Matrix44 world_transform_;
	CollisionModel *collision_model_;
	CollisionModel::Type collision_type_;
	vector<Model*> test_collision_models; // 会发生碰撞的其他物体
};

class Missle:public Model  {
public:

	Vector3 velocity_; // 开始的速度和人的速度一样
	long long ttl_;
	Vector3 rotation; // 各个方向的旋转角度

	Missle(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m):Model(MISSLE, pos, painter, collision_type, m) {
		ttl_ = 0;
	}

	bool isShoot()const {
		return 0<ttl_&&ttl_<MISSLE_TTL; // 5秒
	}
	virtual void draw(const Matrix44& pv) override{ 
		if (isShoot()) {
			Model::draw(pv);
		}
	}
	virtual void update(const Matrix44& vr) override {}
	void reset(const Vector3& pos, const Vector3& enemy_pos) {
		setPos(pos);
		rotation.x = 45.0;
		ttl_ = 1;
	}
	void updateVelocity(const Vector3& dir, double rotation_speed) {
		// 旧的代码
		 //velocity_ = (velocity_ * 0.95 + dir * 0.05).normalize() * 1.0;
		 //rotateZ(MISSLE_ROTATION_SPEED); // 旧的代码，绕着自身的速度方向旋转

		// 可以按照先x再y的顺序，也可以按照先y再x的顺序，但是两次的角度是不一样的
		double y_rotation = atan2(dir.x, dir.z) * 180 / PI;
		double x_rotation = atan2(dir.y, sqrt((dir.x * dir.x + dir.z * dir.z))) * 180 / PI;
		double offset =1.0;
		if (fabs(rotation.y - y_rotation) < offset) {
			rotation.y = y_rotation;
		}
		else if (rotation.y - y_rotation > 0.0) {
			rotation.y -= offset;
		}
		else {
			rotation.y += offset;
		}

		if (fabs(rotation.x - x_rotation) < offset) {
			rotation.x = x_rotation;
		}
		else if (rotation.x - x_rotation > 0.0) {
			rotation.x -= offset;
		}
		else {
			rotation.x += offset;
		}
		rotation.z += rotation_speed;
		setRotationY(rotation.y);
		rotateX(-rotation.x);
		rotateZ(rotation.z);
		velocity_ = getModelRotation().vecMul({ 0,0,1 });
	};
	void update(const Vector3& enemy_pos) {
		if (!isShoot())
			return;
		// 初始速度v0，方向向量d,v0和d有一定的夹角，
		Vector3 dir = enemy_pos - getPos();
		if (dir.norm() < 1.0) {
			ttl_ = 0;
			return;
		}
		updateVelocity(dir, MISSLE_ROTATION_SPEED);
		setPos(getPos() + velocity_);
		if (++ttl_ >= MISSLE_TTL)
			ttl_ = 0;
	}
};

class Stage :public Model
{
public:
	Stage(Type type, Painter* painter, CollisionModel::Type collision_type) :Model(type, { 0.0,0.0,0.0 }, painter, collision_type, Matrix44::identity()) {
		triangles_ = painter->getTriangles();
		initCollisionModel({ 0.0,0.0,0.0 }, { 1000.0, 0.0, 1000.0 }, { 0.0,-10000.0,0.0 }, 10000);
	};
	~Stage() {
	};
	const vector<Triangle>& getTriangles()const {
		return triangles_;
	}
	virtual void update(const Matrix44& vr)override {}
	virtual void draw(const Matrix44& pvm)override {
		Model::draw(pvm);
	}
private:
	vector<Triangle> triangles_;
};
Vector3 getCuboidHalf() {
	return { 10.0,5.0,10.0 };
}
class Mecha :public Model {
public:
	enum State {
		MOVE,
		JUMP_UP,
		JUMP_STAY,
		JUMP_FALL,
		QUICK_MOVE,
	};
	Mecha(Type type, const Vector3& pos, Painter* painter,CollisionModel::Type collision_type, const Matrix44& m = Matrix44::identity()) :Model(type, pos, painter, collision_type, m) {
		// 中心点设置在脚底，因为现在实际上是线段在判断而不是两个球体在判断
		initCollisionModel({pos.x,pos.y+getCuboidHalf().y,pos.z}, getCuboidHalf(), { pos.x,pos.y,pos.z }, getCuboidHalf().y);
		state_ = MOVE;
		enegey_ = 100;
		velocity_ =Vector3();
		jump_count_ = 0;
		enemy_theta_ = 0;
		rotation_y_ = 0;
		rotation_speed_ = 0;
	}
	~Mecha() {
	}
	void setEnemyTheta() {
		const Vector3& enemy_pos = gEnemy->getPos();
		const Vector3& dir = enemy_pos - getPos();
		enemy_theta_ = atan2(dir.x, dir.z)*180.0/PI ;
		if (enemy_theta_ > 360) {
			enemy_theta_ -= 360;
		}
		else if (enemy_theta_ < 0) {
			enemy_theta_ += 360;
		}
	}
	void setRotationSpeed() {
		double delta =enemy_theta_ + rotation_y_; // 因为绕y轴旋转是正方向旋转，R(e)R(y) = R(e+y)
		if (delta > 180.0) // 顺时针旋转rotation 180-delta
			delta -= 360.0;
		else if (delta < -180.0)
			delta += 360.0;
		rotation_speed_ = delta / TURN_DURATION;
	}
	void incrRotationSpeed() {
		if (jump_count_ < TURN_DURATION) {
			rotation_y_ -= rotation_speed_; // 这里要减去，因为是绕着y轴顺时针旋转
			rotateY(rotation_speed_);  
		}
	}
	void stateTransition() {
		Keyboard k = Manager::instance().keyboard();
		/*
		* STILL: wasd->MOVE, space->JUMP_UP, z->QUICK_MOVE
		* MOVE: wasd->MOVE, space->JUMP_UP, +z->QUICK_MOVE, no->STILL
		* QUICK_MOVE: wasd->QUICK_MOVE, space->JUMP_UP, no->STILL, 简化，先去掉这个状态
		* JUMP_UP: arrive top->JUMP_FALL
		* JUMP_FALL: arrive down->STILL 
		*/
		velocity_.y = -1.0; // 重力作用
		switch (state_)
		{
		case MOVE:
			if (k.isOn(' ')) {
				velocity_.y = 1.0;
				state_ = JUMP_UP;
				jump_count_ = 1;
				setRotationSpeed();
				incrRotationSpeed();
			}
			break;
		case JUMP_UP:
			incrRotationSpeed();
			if (jump_count_++ >= MAX_ENEGY) {
				state_ = JUMP_STAY;
			}
			else {
				velocity_.y = 1;
			}
			break;
		case JUMP_STAY:
			incrRotationSpeed();
			if (jump_count_++ == MAX_ENEGY+SKY_STAY) {
				velocity_.y = -1;
				state_ = JUMP_FALL;
			}
			else {
				velocity_.y = 0;
			}
			break;
		case JUMP_FALL:
			incrRotationSpeed();
			if (jump_count_++ == MAX_ENEGY+SKY_STAY+FALL_DURATION) {
				state_ = MOVE;
			}
			break;
		}
	}
	void updateVelocity(const Matrix44&vr) {
		Keyboard k = Manager::instance().keyboard();
		Vector3 move;
		if (k.isOn('w')) { // 这里设置坐标值为y轴的两倍似乎比较合理，但是没有严格的数学推导
			move.z = 1.0;
		}
		if (k.isOn('a')) {
			move.x= -1.0;
		}
		if (k.isOn('s')) {
			move.z= -1.0;
		}
		if (k.isOn('d')) {
			move.x= 1.0;
		}
		// 仅保留水平分量 |v|cos t * a/|a|  = v.dot(a) / |a|^2 *a
		Vector3 viewMove = vr.vecMul(move);
		double cur_speed = velocity_.norm();
		if (viewMove.x == 0 && viewMove.z == 0 && cur_speed >FRAME_SPEED_ACC || velocity_.dot(viewMove)<0) { // 修复反向的时候速度还是不减少的问题
			velocity_ =velocity_.normalize()*(cur_speed - FRAME_SPEED_ACC);
			return;
		}

		if (velocity_.dot(viewMove) == 0) {
			velocity_ = viewMove* MAX_SPEED / (ACC_DURATION * FRAMES);
			return;
		}
		// 当前因为相机是向y轴负方向的，所以沿着z方向的速度不可能达到最大值，他只会一直减少，所以下面没有除以他的范数，而是把y忽略掉了
		double viewMoveSqureDist = (viewMove.x * viewMove.x + viewMove.z * viewMove.z);
		velocity_ = viewMove * velocity_.dot(viewMove) / viewMoveSqureDist;
		cur_speed = velocity_.norm();
		if (cur_speed + FRAME_SPEED_ACC <= MAX_SPEED)
			velocity_ =velocity_.normalize()* (cur_speed+FRAME_SPEED_ACC);

	}
	void collisionTest() {
		// 存在一个方向，使得和其他所有物体都不相撞，才可以移动
// 反之，存在一个物体，所有方向都和他相撞，则不可以移动
// TODO 目前的处理存在抖动现象，相当于说每帧物体的移动方向都会发生改变，比如在爬很抖的坡的时候，一会儿向前，一会儿向后
		const Vector3 old_pos = getPos();
		if (getCollsionModel()->getType() == CollisionModel::Type::CUBOID) {
			vector<Vector3> possible_move_vectors = {
			velocity_,
			{0.0,velocity_.y,velocity_.z},
			{velocity_.x,0.0,velocity_.z},
			{velocity_.x,velocity_.y,0.0},
			{0.0,0.0,velocity_.z},
			{0.0,velocity_.y,0.0},
			{velocity_.x,0.0,0.0}
			};
			for (auto& v : possible_move_vectors) {
				updateCollisionPos(old_pos + v);
				bool could_move = true;
				for (auto& other_model : getCollisionModels()) {
					if (isCollision(other_model)) {
						could_move = false;
						break;
					}
				}
				if (could_move) {
					setPos(old_pos + v);
					break;
				}
			}
		}
		else if (getCollsionModel()->getType() == CollisionModel::Type::SPHERE) {
			Vector3 old_origin = getCollsionModel()->getOrigin();
			bool keep_origin = false;
			auto tri_loop_test = [&](Model* other_model) {
				const Stage& o = dynamic_cast<const Stage&> (*other_model);
				for (auto& tri : o.getTriangles()) {
					if (tri.isCollision(old_origin, velocity_)) {
						keep_origin = true;
						break;
					}
				}
				};

			for (auto& other_model : getCollisionModels()) {
				updateCollisionPos(old_origin + velocity_);
				if (other_model->getCollsionModel()->getType() == CollisionModel::SPHERE && isCollision(other_model)) {
					Vector3 t = other_model->getCollsionModel()->getOrigin() - old_origin;
					double s = 1 / t.squareDist();
					velocity_ -= t * (velocity_.dot(t)) * (1 / t.squareDist());
				}
				else if (other_model->getCollsionModel()->getType() == CollisionModel::TRIANGLE) { // 碰撞检测的部分可以继续优化，这部分写的不太优雅
					const Stage& o = dynamic_cast<const Stage&> (*other_model);
					for (auto& tri : o.getTriangles()) {
						if (tri.isCollision(old_origin, velocity_)) {
							Vector3 n = tri.getNorm();
							double lambda = n.dot(velocity_) / n.dot(n);
							velocity_ -= n * lambda;
						}
					}
					// (*) 三角形是数组，因此要循环，
					// 为了避免间隙处的穿透问题，使用两次循环，第一次循环如果没有发生碰撞，则直接使用就可以
					// 如果发生了碰撞，使用校正后的向量进行第二次循环，因此本次是不会和之前已经碰撞修复过的再碰撞，如果还是发生了碰撞则不可以使用这次的移动，否则会穿透之前的物体
					tri_loop_test(other_model);
				}
			}
			// 同注释（*），对多个物体循环两次
			for (auto& other_model : getCollisionModels()) {
				if (other_model->getCollsionModel()->getType() == CollisionModel::TRIANGLE) {
					tri_loop_test(other_model);
				}
			}
			if (keep_origin) {
				updateCollisionPos(old_origin); // 之前的bug是由与没有同步更新这个向量导致
			}
			else {
				updateCollisionPos(old_origin + velocity_);
				setPos(old_pos + velocity_);
			}
		}
	}
	void attackHandle() {
		Keyboard k = Manager::instance().keyboard();
		if (k.isTriggered('j')) {
			for (int i = 0; i < MAX_MISSLES; i++) {
				if (!missles_[i].isShoot()) { 
					missles_[i].reset(getPos(),gEnemy->getPos());
					break;
				}
			}
		}

		for (int i = 0; i < MAX_MISSLES; i++) {
			missles_[i].update(gEnemy->getPos());
		}
	}
	virtual void update(const Matrix44& vr)override {
		// 这里的移动是在相机坐标系内移动
		// 移动前坐标为Y，世界坐标X = AY+C，
		// 移动后坐标为Y',世界坐标X' = AY'+C,
		// 两式相减X'-X = A(Y'-Y) => X'=X+A(Y'-Y) => X'=X+A delta，其中delta是相机坐标系中的位移量
		// 或者 X=AY+C => X=A(Y+delta)+c => X=AY + c +A delta，增加量还是旋转*delta
		updateVelocity(vr);
		stateTransition();
		collisionTest();
		attackHandle();
		setEnemyTheta();
		printDebugInfo();
	}
	void addMissle(Model& missle){
		missles_.push_back(dynamic_cast<Missle&> (missle));
	
	}
	virtual void draw(const Matrix44& pv)override {
		Model::draw(pv);
		for (int i = 0; i < MAX_MISSLES; i++) {
			missles_[i].draw(pv);
		}

	}
	void addMissle(Missle *missle) {
		missles_.push_back(*missle);
	}
private:
	State state_;
	Vector3 velocity_;
	int jump_count_;
	int enegey_; // 当前的能量条
	double enemy_theta_; // 和敌人的角度
	double rotation_y_; // 绕着Y轴的旋转角
	double rotation_speed_; // 绕着Y轴旋转的速度
	vector<Missle> missles_;

	void printDebugInfo() {
		ostringstream oss;
		oss << "player pos: " << getPos()<<" , enemy_pos: "<<gEnemy->getPos();
		Framework::instance().drawDebugString(0, 1, oss.str().c_str());
		oss.str("");
		oss << "rotation_y: " << rotation_y_ << ", enemy_theta: " << enemy_theta_<<"rotation speed: "<< rotation_speed_;
		Framework::instance().drawDebugString(0, 2, oss.str().c_str());
		oss.str("");
		oss << "velocity: "<< velocity_ << ", norm: " << velocity_.norm();
		Framework::instance().drawDebugString(0, 3, oss.str().c_str());
		oss.str("");
		oss <<"missle pos: "<<missles_[0].getPos()<<", dis: "<<(missles_[0].getPos() - gEnemy->getPos()).norm();
		Framework::instance().drawDebugString(0, 4, oss.str().c_str());
		oss.str("");
	}
};

class Enemy :public Model {
public:
	Enemy(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m = Matrix44::identity()) :Model(type, pos, painter, collision_type, m) {
		// 中心点设置在脚底，因为现在实际上是线段在判断而不是两个球体在判断
		initCollisionModel({ pos.x,pos.y + getCuboidHalf().y,pos.z }, getCuboidHalf(), { pos.x,pos.y,pos.z }, getCuboidHalf().y);
	}
	~Enemy() {
	}
	virtual void update(const Matrix44& vr)override {
		setPos(getPos().x + (rand()%100-5.0) /FRAMES, getPos().y, getPos().z+ (rand() % 100-5.0) / FRAMES);
	}
	virtual void draw(const Matrix44& pv)override {
		Model::draw(pv);
	}
};
// 绘制辅助坐标轴
class Axis :public Model {
public:
	Axis(Type type, Painter* painter, CollisionModel::Type collision_type) :Model(type, { 0.0,0.0,0.0 }, painter, collision_type, Matrix44::identity()) {}
	~Axis() {}
	virtual void update(const Matrix44& pvm) override {}
	virtual void draw(const Matrix44& pv)override {
		Model::draw(pv);
	}
};

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
				new_model = new Enemy(type, origin, p, collision_type);
			else if (type == Model::STAGE)
				new_model = new Stage(type, p, collision_type);
			else if (type == Model::AXIS)
				new_model = new Axis(type, p, collision_type);
			else if (type == Model::MISSLE)
				new_model = new Missle(type,origin,p,collision_type, Matrix44::identity());
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
	void update(Model* player) {
		const Vector3& origin = player->getPos();
		const Vector3& z_dir = player->getZDirection();
		// 从模型的后六米看向他前面的10米位置
		eye_pos_ = player->getModelTransform().vecMul({ 0,20,-60 });
		target_pos_ = player->getModelTransform().vecMul({ 0,0,10 });
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
