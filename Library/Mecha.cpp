#include "pch.h"
#include "include/Library/Mecha.h"
#include <GameLib/Input/Manager.h>
#include "include/Library/Model.h"
#include "include/Library/Missle.h"
#include "include/Library/TransformTree.h"
#include "include/Library/Resource.h"
#include "GameSound.h"

// 移动速度
const double MAX_SPEED = 4.0;
const double ACC_DURATION = 2.0; // 单位秒
double FRAME_SPEED_ACC = MAX_SPEED / (ACC_DURATION * FRAMES);
extern Resource* gResource;

Mecha::Mecha(Type type, const Vector3& pos, Painter* painter, CollisionModel::Type collision_type, const Matrix44& m):Model(type, pos, painter, collision_type, m)
{
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
	frame_input_ = new FrameInput;
	transform_tree_ = gResource->createTransformTree("player");
	//GameSound::instance().playBGM(GameSound::MECHA_THEME);

}
// 下面的两个函数不能加上inline关键字,因为是public函数，内联函数的定义必须放在
void Mecha::update(const Matrix44& vr)
{
	if (!isAlive())
		return;
		// 这里的移动是在相机坐标系内移动
		// 移动前坐标为Y，世界坐标X = AY+C，
		// 移动后坐标为Y',世界坐标X' = AY'+C,
		// 两式相减X'-X = A(Y'-Y) => X'=X+A(Y'-Y) => X'=X+A delta，其中delta是相机坐标系中的位移量
		// 或者 X=AY+C => X=A(Y+delta)+c => X=AY + c +A delta，增加量还是旋转*delta
	frame_input_->update();
	AI();
	updateVelocity(vr);
	stateTransition();
	attackHandle();
	collisionTest();
	setEnemyTheta();
	lockOn();
	printDebugInfo();
	transform_tree_->update();
}
void Mecha::addMissle(Model& missle)
{
	missles_.push_back(dynamic_cast<Missle&> (missle));
}
void Mecha::addEnemy(Model* enemy)
{
	enemy_ = enemy;
}
void Mecha::draw(const Matrix44& pv, const Light* light)
{
	transform_tree_->draw(pv, getModelTransform(), const_cast<Light*>(light));
	for (int i = 0; i < missles_.size(); i++) {
		missles_[i].draw(pv, light);
	}
}
bool Mecha::isAlive() const
{
	return hp_ > 0;
}
int Mecha::getHP() const
{
	return hp_;
}
void Mecha::getDamage()
{
	hp_ -= MISSLE_ENGEY_COST;
}
void Mecha::printDebugInfo() const
{
	if (getType() != PLAYER)
		return;
	//int i = 0;
	//ostringstream oss;
	//oss << "player pos: " << getPos() << " , enemy_pos: " << gEnemy->getPos();
	//Framework::instance().drawDebugString(0, i++, oss.str().c_str());
	//oss.str("");
	////oss << "rotation_y: " << rotation_y_ << ", enemy_theta: " << enemy_theta_<<"rotation speed: "<< rotation_speed_;
	////Framework::instance().drawDebugString(0, 2, oss.str().c_str());
	////oss.str("");
	////oss << "velocity: "<< velocity_ << ", norm: " << velocity_.norm()<<", time: " << gCounter*1.0/FRAMES;
	////Framework::instance().drawDebugString(0, 3, oss.str().c_str());
	////oss.str("");
	////oss <<"missle pos: "<<missles_[0].getPos()<<", dis: "<<(missles_[0].getPos() - gEnemy->getPos()).norm();
	////Framework::instance().drawDebugString(0, 4, oss.str().c_str());
	////oss.str("");
	//oss << "player hp: " << dynamic_cast<Mecha*>(gPlayer)->getHP() << "player energy: "<< dynamic_cast<Mecha*>(gPlayer)->energy_ <<", enemy hp: " << dynamic_cast<Mecha*>(gEnemy)->getHP()<<", enermy energe: "<< dynamic_cast<Mecha*>(gEnemy)->energy_;
	//Framework::instance().drawDebugString(0, i++, oss.str().c_str());
	//oss.str("");
	//oss << "lock on: " << lock_on_;
	//Framework::instance().drawDebugString(0, i++, oss.str().c_str());
	//oss.str("");

}
void Mecha::collisionTest()
{
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
void Mecha::attackHandle()
{
	bool isAttack = frame_input_->is_FIRE && !(state_ == JUMP_UP || state_ == JUMP_FALL) && energy_ >= MISSLE_ENGEY_COST;
	if (isAttack)
		fire();
	else
		recoverEnergy();
	for (int i = 0; i < MAX_MISSLES; i++) {
		missles_[i].update(enemy_);
	}
}
void Mecha::recoverEnergy()
{
	if (energy_ + ENEGY_RECOVER <= MAX_ENEGY)
		energy_ += ENEGY_RECOVER;
}
void Mecha::setEnemyTheta()
{
	const Vector3& enemy_pos = enemy_->getPos();
	const Vector3& dir = enemy_pos - getPos();
	enemy_theta_ = atan2(dir.x, dir.z) * 180.0 / PI;
	if (enemy_theta_ > 360) {
		enemy_theta_ -= 360;
	}
	else if (enemy_theta_ < 0) {
		enemy_theta_ += 360;
	}
}
void Mecha::setRotationSpeed()
{
	double delta = enemy_theta_ + rotation_y_; // 因为绕y轴旋转是正方向旋转，R(e)R(y) = R(e+y)
	if (delta > 180.0) // 顺时针旋转rotation 180-delta
		delta -= 360.0;
	else if (delta < -180.0)
		delta += 360.0;
	rotation_speed_ = delta / ZOOM_DURATION;
}
void Mecha::incrRotationSpeed()
{
	if (jump_count_ < ZOOM_DURATION) {
		rotation_y_ -= rotation_speed_; // 这里要减去，因为是绕着y轴顺时针旋转
		rotateY(rotation_speed_);
	}
}
void Mecha::stateTransition()
{
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
	case STANDING:
		if (frame_input_->isMove()) {
			state_ = MOVE;
			transform_tree_->setAnimation(gResource->getAnimation("walking"));
		}
		if (frame_input_->is_LEFT_ROTATE) { // 注意这里要同步更新rotation_y,不然后续不更新，按空格就没用了
			rotateY(TURN_DEGREE);
			rotation_y_ -= TURN_DEGREE;
		}
		else if (frame_input_->is_RIGHT_ROTATE) {
			rotateY(-TURN_DEGREE);
			rotation_y_ += TURN_DEGREE;
		}
		else if (frame_input_->is_LOCK) {
			rotateY(enemy_theta_ + rotation_y_);
			rotation_y_ -= enemy_theta_ + rotation_y_;
		}
		break;
	case MOVE:
		if (frame_input_->is_JUMP) {
			velocity_.y = 1.0;
			state_ = JUMP_UP;
			jump_count_ = 1;
			setRotationSpeed();
			incrRotationSpeed();
			transform_tree_->setAnimation(gResource->getAnimation("jumping"));
		}
		if (!frame_input_->isMove()) {
			state_ = STANDING;
			transform_tree_->setAnimation(gResource->getAnimation("standing"));
		}
		if (frame_input_->is_LEFT_ROTATE) { // 注意这里要同步更新rotation_y,不然后续不更新，按空格就没用了
			rotateY(TURN_DEGREE);
			rotation_y_ -= TURN_DEGREE;
		}
		else if (frame_input_->is_RIGHT_ROTATE) {
			rotateY(-TURN_DEGREE);
			rotation_y_ += TURN_DEGREE;
		}
		break;
	case JUMP_UP:
		incrRotationSpeed();
		if (jump_count_++ >= JUMP_UP_DURATION) {
			state_ = JUMP_STAY;
		}
		else {
			velocity_.y = 1;
		}
		break;
	case JUMP_STAY:
		incrRotationSpeed();
		if (jump_count_++ == JUMP_UP_DURATION + SKY_STAY) {
			velocity_.y = -1;
			state_ = JUMP_FALL;
		}
		else {
			velocity_.y = 0;
		}
		break;
	case JUMP_FALL:
		incrRotationSpeed();
		if (jump_count_++ == JUMP_UP_DURATION + SKY_STAY + FALL_DURATION) {
			state_ = STANDING;
			transform_tree_->setAnimation(gResource->getAnimation("standing"));
		}
		break;
	}
}
void Mecha::updateVelocity(const Matrix44& vr)
{
	Vector3 move;
	if (frame_input_->is_UP) { // 这里设置坐标值为y轴的两倍似乎比较合理，但是没有严格的数学推导
		move.z = 1.0;
	}
	if (frame_input_->is_LEFT) {
		move.x = -1.0;
	}
	if (frame_input_->is_DOWN) {
		move.z = -1.0;
	}
	if (frame_input_->is_RIGHT) {
		move.x = 1.0;
	}
	// 仅保留水平分量 |v|cos t * a/|a|  = v.dot(a) / |a|^2 *a
	// 剔除掉相机的z分量
	Vector3 viewMove = vr.vecMul(move); 

	double cur_speed = velocity_.norm();
	if (viewMove.x == 0 && viewMove.z == 0 && cur_speed > FRAME_SPEED_ACC || velocity_.dot(viewMove) < 0) { // 修复反向的时候速度还是不减少的问题
		velocity_ = velocity_.normalize() * (cur_speed - FRAME_SPEED_ACC);
		return;
	}

	if (velocity_.dot(viewMove) == 0) {
		velocity_ = viewMove * MAX_SPEED / (ACC_DURATION * FRAMES);
		return;
	}
	// 当前因为相机是向y轴负方向的，所以沿着z方向的速度不可能达到最大值，他只会一直减少，所以下面没有除以他的范数，而是把y忽略掉了
	double viewMoveSqureDist = (viewMove.x * viewMove.x + viewMove.z * viewMove.z);
	velocity_ = viewMove * velocity_.dot(viewMove) / viewMoveSqureDist;
	cur_speed = velocity_.norm();
	if (cur_speed + FRAME_SPEED_ACC <= MAX_SPEED)
		velocity_ = velocity_.normalize() * (cur_speed + FRAME_SPEED_ACC);
}
void Mecha::lockOn()
{ // 瞬时锁定功能
	Vector3 enemy_dir = (enemy_->getPos() - getPos());
	Vector3 z_dir = getModelRotation().vecMul({ 0,0,1 }).normalize();

	double theta = acos(enemy_dir.dot(z_dir) / enemy_dir.norm()) * 180.0 / PI;
	using std::to_string;
	GameLib::Framework::instance().drawDebugString(1, 8, to_string((int)theta).c_str());
	if (theta > MIN_LOCK_OFF && lock_on_)
		lock_on_ = false;
	else if (theta <= MAX_LOCK_ON && !lock_on_)
		lock_on_ = true;
}
void Mecha::fire()
{
	for (int i = 0; i < MAX_MISSLES; i++) {
		if (!missles_[i].isShoot()) {
			missles_[i].reset(getPos(), enemy_->getPos());
			energy_ -= MISSLE_ENGEY_COST;
			break;
		}
	}
}
void Mecha::AI()
{
	if (getType() == PLAYER)
		return;
	frame_input_->reset();
	Mecha *enemy =dynamic_cast<Mecha*>(enemy_);
	if (enemy->frame_input_->is_JUMP) // 玩家跳跃则攻击
	{
		frame_input_->is_FIRE = true;
	}
	int up = rand() % 2;
	int left = rand() % 2;
	int right = rand() % 2;
	int down = rand() % 2;
	Vector3 move = enemy_->getPos() - getPos();
	if (move.x > 0)
		right = true;
	else
		left = true;
	if (move.z > 0)
		up = true;
	else
		down = true;
	frame_input_->is_UP = up;
	frame_input_->is_LEFT = left;
	frame_input_->is_RIGHT= right;
	frame_input_->is_DOWN = down;
	frame_input_->is_FIRE = rand()%1000==0;
}