#pragma once
#include "Model.h"
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
		// 从模型的后看
		eye_pos_ = player->getModelTransform().vecMul({ 0,10,-20 });
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