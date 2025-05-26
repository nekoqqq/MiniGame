#pragma once
#include "Model.h"
class Missle :public Model {
public:

	Vector3 velocity_; // 开始的速度和人的速度一样
	long long ttl_;
	Vector3 rotation; // 各个方向的旋转角度

	Missle(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m) :Model(MISSLE, pos, painter, collision_type, m) {
		ttl_ = 0;
	}

	bool isShoot()const;

	void draw(const Matrix44& pv, const Light* light) override;
	void update(const Matrix44& vr) override;

	void reset(const Vector3& pos, const Vector3& enemy_pos);

	void updateVelocity(const Vector3& dir, double rotation_speed);
	void update(Model* enemy);
};
