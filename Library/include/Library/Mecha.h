#pragma once
#include "Model.h"
#include "Missle.h"

class Mecha :public Model {
public:
	enum State {
		MOVE,
		JUMP_UP,
		JUMP_STAY,
		JUMP_FALL,
		QUICK_MOVE,
		TURNING, // 转身
		ATTACKING, // 攻击状态
	};
	Mecha(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m = Matrix44::identity()) :Model(type, pos, painter, collision_type, m) {
		// 中心点设置在脚底，因为现在实际上是线段在判断而不是两个球体在判断
		initCollisionModel({ pos.x,pos.y + getCuboidHalf().y,pos.z }, getCuboidHalf(), { pos.x,pos.y,pos.z }, getCuboidHalf().y);
		state_ = MOVE;
		energy_ = MAX_ENEGY;
		velocity_ = Vector3();
		jump_count_ = 0;
		enemy_theta_ = 0;
		rotation_y_ = 0;
		rotation_speed_ = 0;
		hp_ = MAX_HP;
		lock_on_ = false;
		enemy_ = nullptr;
	}
	~Mecha() {}
	void update(const Matrix44& vr)override;
	void addMissle(Model& missle);
	void addEnemy(Model* enemy);
	void draw(const Matrix44& pv)override;
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
protected:
	void printDebugInfo()const ;
	void collisionTest();
	void fire();
	void attackHandle(bool isAttack);
	void recoverEnergy();
	void setEnemyTheta();
	void setRotationSpeed();
	void incrRotationSpeed();
	void stateTransition();
	void updateVelocity(const Matrix44& vr);
	void lockOn();
	void AI();
};