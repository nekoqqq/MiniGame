#pragma once
#include <GameLib/Input/Manager.h>

#include "Model.h"
#include "Missle.h"
class TransformTree;
class Resource;
struct FrameInput
{
	FrameInput():is_UP(false),is_LEFT(false),is_DOWN(false),is_RIGHT(false),is_JUMP(false),is_FIRE(false),is_LEFT_ROTATE(false),is_RIGHT_ROTATE(false){}
	void reset()
	{
		is_UP = false;
		is_LEFT = false;
		is_DOWN = false;
		is_RIGHT = false;
		is_JUMP = false;
		is_FIRE = false;
		is_LEFT_ROTATE = false;
		is_RIGHT_ROTATE = false;
	}
	void update()
	{
		Keyboard k = Manager::instance().keyboard();
		is_UP = k.isOn('w');
		is_LEFT = k.isOn('a');
		is_DOWN = k.isOn('s');
		is_RIGHT = k.isOn('d');
		is_JUMP = k.isOn(' ');
		is_FIRE = k.isTriggered('j');
		is_LEFT_ROTATE = k.isOn('i');
		is_RIGHT_ROTATE = k.isOn('u');
	}
	bool isMove()const {
		return is_UP || is_LEFT || is_DOWN || is_RIGHT || is_JUMP;
	}
	bool is_UP;
	bool is_LEFT;
	bool is_DOWN;
	bool is_RIGHT;
	bool is_JUMP;
	bool is_FIRE;
	bool is_LEFT_ROTATE;
	bool is_RIGHT_ROTATE;
};
class Mecha :public Model {
public:
	friend class MechaInfo; //TODO 这里最好不要用友元类实现
	enum State {
		STANDING, // 静止
		MOVE,
		JUMP_UP,
		JUMP_STAY,
		JUMP_FALL,
		QUICK_MOVE,
		TURNING, // 转身
		ATTACKING, // 攻击状态
	};
	Mecha(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m = Matrix44::identity());

	~Mecha()
	{
		SAFE_DELETE(frame_input_);
	}
	void setTransformTree(TransformTree* tree);
	void update(const Matrix44& vr)override;
	void addMissle(Model& missle);
	void addEnemy(Model* enemy);
	void draw(const Matrix44& pv, const Light* light)override;
	bool isAlive()const;
	int getHP()const;
	void getDamage();
private:
	State state_;
	Vector3 velocity_;
	int jump_count_;
	double enemy_theta_; // 和敌人的角度
	double rotation_y_; // 绕着Y轴的旋转角
	double rotation_speed_; // 绕着Y轴旋转的速度
	vector<Missle> missles_;
	int energy_; // 当前的能量条
	int hp_;
	bool lock_on_; // 是否锁定了敌人
	Model* enemy_; // 敌人
	FrameInput* frame_input_; // 当前帧的输入
	TransformTree* transform_tree_;
protected:
	void printDebugInfo()const ;
	void collisionTest();
	void fire();
	void attackHandle();
	void recoverEnergy();
	void setEnemyTheta();
	void setRotationSpeed();
	void incrRotationSpeed();
	void stateTransition();
	void updateVelocity(const Matrix44& vr);
	void lockOn();
	void AI();
};