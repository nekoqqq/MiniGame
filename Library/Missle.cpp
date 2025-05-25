#include "pch.h"
#include "include/Library/Missle.h"
#include "include/Library/Mecha.h"

bool Missle::isShoot() const
{
	return 0 < ttl_ && ttl_ < MISSLE_TTL; // 5秒
}

void Missle::draw(const Matrix44& pv)
{
	if (isShoot()) {
		Model::draw(pv);
	}
}

void Missle::update(const Matrix44& vr)
{}

void Missle::reset(const Vector3& pos, const Vector3& enemy_pos)
{
	setPos(pos);
	rotation.x = 45.0;
	ttl_ = 1;
}

void Missle::update(const Vector3& enemy_pos)
{
	if (!isShoot())
		return;
	// 初始速度v0，方向向量d,v0和d有一定的夹角，
	Vector3 dir = enemy_pos - getPos();
	if (dir.norm() < 5.0) {
		ttl_ = 0;
		dynamic_cast<Mecha*>(gEnemy)->getDamage();
		return;
	}
	updateVelocity(dir, MISSLE_ROTATION_SPEED);
	setPos(getPos() + velocity_);
	if (++ttl_ >= MISSLE_TTL) // 子弹消失
		ttl_ = 0;
}

void Missle::updateVelocity(const Vector3&dir, double rotation_speed)
{
	// 旧的代码
		 //velocity_ = (velocity_ * 0.95 + dir * 0.05).normalize() * 1.0;
		 //rotateZ(MISSLE_ROTATION_SPEED); // 旧的代码，绕着自身的速度方向旋转

		// 可以按照先x再y的顺序，也可以按照先y再x的顺序，但是两次的角度是不一样的
	double y_rotation = atan2(dir.x, dir.z) * 180 / PI;
	double x_rotation = atan2(dir.y, sqrt((dir.x * dir.x + dir.z * dir.z))) * 180 / PI;
	double offset = 1.0;
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
}