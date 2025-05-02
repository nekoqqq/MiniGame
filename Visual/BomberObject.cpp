#include "../Console/GameObject.h"
#include "VisualGame.h"
#include <vector>
#include "../Console/DDS.h"
#include "BomberObject.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <ctime>
#include "GameLib/Framework.h"
#include "BomberGame.h"

using std::pair;
using std::vector;

BomberObject::BomberObject() {
	obj_img_ = std::make_shared<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\bomber.dds");
	type_ = GROUND;
	power_ = 1;
	expired_ = false;
}
BomberObject::BomberObject(int x) : r_(PIXCEL_SIZE / 2) {
	this->type_ = static_cast<Type> (x);
}
BomberObject::BomberObject(int x_, int y_, int r_) {
	this->x_ = x_;
	this->y_ = y_;
	this->r_ = r_;
}
BomberObject::BomberObject(int i, int j, Type type) {
	x_ = i * PIXCEL_SIZE + PIXCEL_SIZE / 2;
	y_ = j * PIXCEL_SIZE + PIXCEL_SIZE / 2;
	r_ = PIXCEL_SIZE / 2;
	type_ = type;
	if (type_ == P1_PLAYER || type_ == P2_PLAYER)
		r_ -= 2;


	obj_img_ = std::make_shared<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\bomber.dds");
	power_ = 1;
	expired_ = false;
	direction = UNKNOWN;
	put_time_ = 0;
}
bool BomberObject::isDestroyed() const {
	return !expired_;
}
pair<int, int> BomberObject::softPos() const {
	return { 8 / 4,8 % 4 };
}
pair<int, int> BomberObject::bombPos() const {
	return { 0,2 };
}
void BomberObject::drawAtScreen() const {
	int i = getType() / 4;
	int j = getType() % 4;
	pair<int, int> soft_pos = softPos();
	pair<int, int> bomb_pos = bombPos();
	if (getType() == BOMB_RED || getType() == BOM_BLUE) {
		if (1 || isDestroyed())
			obj_img_->drawAtScreen(i, j, (int)(x_ - r_), (int)(y_ - r_));
		else
			obj_img_->drawAtScreen(soft_pos.first, soft_pos.second, (int)(x_ - r_), (int)(y_ - r_));
	}
	else
		obj_img_->drawAtScreen(i, j, (int)(x_ - r_), (int)(y_ - r_));
}
void BomberObject::operator=(int x) {
	this->type_ = static_cast<Type>(x);
}
bool BomberObject::validPos() {
	return 0 <= (x_-r_) && (x_ - r_) < GRID_HEIGHT * PIXCEL_SIZE && 0 <= (y_ - r_) && (y_ - r_) < GRID_WIDHT * PIXCEL_SIZE;
}
bool BomberObject::validIndex(int i,int j) {
	return 0 <= i && i < GRID_HEIGHT && 0 <= j && j < GRID_WIDHT;
}
void BomberObject::move(int dx, int dy) {
	if (type_ != P1_PLAYER && type_ != P2_PLAYER && type_ != ENEMY)
		return;

	int raw_i = getInnerPos().first;
	int raw_j = getInnerPos().second;

	double move_x = dx * PLAYER_SPEED;
	double move_y = dy * PLAYER_SPEED;
	if (getType() == ENEMY) { // 敌人随机移动
		move_x = Framework::instance().getRandom() % 2 * ENEMY_SPEED;
		move_y = Framework::instance().getRandom() % 2 * ENEMY_SPEED;
	}
	// TODO 这里用了总共18次循环，但是可以优化成只判断4个格子
	// x方向
	x_ += move_x;
	bool is_x_hit = false;
	for (int i = raw_i - 1; i < raw_i + 2; i ++) {
		for (int j = raw_j - 1; j < raw_j + 2; j++) {
			if (!validIndex(i, j))
				continue;
			BomberObject& t = BomberGame::instance().getGameObject(i, j);
			if ((t.type_ == IRON_WALL || t.type_==BOMB) && isCollision(t)) {
				is_x_hit = true;
			}
		}
	}
	for (auto d : BomberGame::instance().getDynamicObject()) {
		if (d.getType() == BOMB && isCollision(d))
			is_x_hit = true;
	}
	x_ -= move_x;


	y_ += move_y;
	bool is_y_hit = false;
	// TODO 现在检查的是周围9个的网格，可以优化到只检查4个网格
	for (int i = raw_i - 1; i < raw_i + 2; i++) {
		for (int j = raw_j - 1; j < raw_j + 2; j ++) {
			if (!validIndex(i, j))
				continue;
			BomberObject& t = BomberGame::instance().getGameObject(i, j);
			if ((t.type_ == IRON_WALL || t.type_ == BOMB) && isCollision(t)) {
				is_y_hit = true;
			}

		}
	}
	for (auto d : BomberGame::instance().getDynamicObject()) {
		if (d.getType() == BOMB && isCollision(d))
			is_y_hit = true;
	}
	y_ -= move_y;

	if (is_x_hit && !is_y_hit) {
		y_ += move_y;
		direction = dy > 0 ? EAST : WEST;
	}
	else if (!is_x_hit && is_y_hit) {
		x_ += move_x;
		direction = dx > 0 ? SOUTH : NORTH;
	}
	else {
		x_ += move_x;
		y_ += move_y;
		bool is_hit = false;
		for (int i = raw_i - 1; i < raw_i + 2; i++) {
			for (int j = raw_j - 1; j < raw_j + 2; j++) {
				if (!validIndex(i, j))
					continue;
				BomberObject& t = BomberGame::instance().getGameObject(i, j);
				if ((t.type_ == IRON_WALL || t.type_ == BOMB) && isCollision(t)) {
					is_hit = true;
				}
			}
		}
		for (auto d : BomberGame::instance().getDynamicObject()) {
			if (d.getType() == BOMB && isCollision(d))
				is_hit = true;
		}
		x_ -= move_x;
		y_ -= move_y;
		
		if (!is_hit) {
			x_ += move_x;
			y_ += move_y;
			direction = dy > 0 ? EAST : WEST;
		}
	}

}
bool BomberObject::isCollision(BomberObject& o) {
	double a_l = x_ - r_;
	double a_r = x_ + r_;
	double o_l = o.x_ - r_;
	double o_r = o.x_ + r_;
	if (a_l < o_r && a_r > o_l) {
		double a_t = y_ - r_;
		double a_b = y_ + r_;
		double o_t = o.y_ - o.r_;
		double o_b = o.y_ + o.r_;
		if (a_t < o_b && a_b > o_t)
			return true;
	}
	return false;
}
void BomberObject::setCoordinate(double x, double y) {
	x_ = x;
	y_ = y;
	r_ = PIXCEL_SIZE / 2;
	if (getType() == P1_PLAYER || getType() == P2_PLAYER)
		r_ -= 2;

}
BomberObject::Type BomberObject::getType()const {
	return type_;
}
void BomberObject::setType(Type type) {
	type_ = type;
}

void BomberObject::setPutTime(unsigned t) {
	put_time_ = t;
}

void BomberObject::drawAt(int screen_i,int screen_j)const {
    int i = getType() / 4;
    int j = getType() % 4;
	obj_img_->drawFrom(i, j, screen_i, screen_j);
}
BomberObject* BomberObject::createBomb() {
	int i = getInnerPos().first;
	int j = getInnerPos().second;

	int raw_i = i;
	int raw_j = j;

	switch (direction)
	{
	case BomberObject::NORTH:
		i -= 1;
		break;
	case BomberObject::EAST:
		j += 1;
		break;
	case BomberObject::SOUTH:
		i += 1;
		break;
	case BomberObject::WEST:
		j -= 1;
		break;
	}

	BomberObject* new_bomb = nullptr;
	BomberObject& bo = BomberGame::instance().getGameObject(i, j);
	if (bo.type_ == GROUND) {
		new_bomb= new BomberObject(i,j,BOMB);
		new_bomb->setPutTime(Framework::instance().time());
		x_ = raw_i * PIXCEL_SIZE + r_;
		y_ = raw_j * PIXCEL_SIZE + r_;
	}
	return new_bomb;
}
pair<int, int> BomberObject::getInnerPos() const
{
	int i = x_ / PIXCEL_SIZE;
	int j = y_ / PIXCEL_SIZE;
	return pair<int, int>(i,j);
}
void BomberObject::kill() {
	expired_ = true;
}
bool BomberObject::destroyable() const {
	if (type_ == P1_PLAYER || type_ == P2_PLAYER || type_ == SOFT_WALL || type_==BOMB|| type_ == ENEMY)
		return true;
	return false;
}
bool BomberObject::playerable() const
{
	if(type_ == P1_PLAYER || type_ == P2_PLAYER)
		return true;
	return false;
}
bool BomberObject::shockwavable() const
{
	return type_ == EXPLOSION_H || type_ == EXPLOSION_V;
}
bool BomberObject::explodable() const
{
	return type_ == BOMB;
}
bool BomberObject::shouldExplode() const {
	return type_ == BOMB && Framework::instance().time() - put_time_ > 3000;
}

bool BomberObject::isAlive() const {
	return !expired_;
}

bool BomberObject::movable() const {
	if (type_ == P1_PLAYER || type_ == P2_PLAYER || type_ == ENEMY)
		return true;
	return false;
}


void BomberObject::explode() {
	expired_ = true;
	pair<int, int > inner_pos = getInnerPos();
	int power = BomberGame::instance().getBomberPower();

	// 设置爆炸中心
	BomberObject* bo = new BomberObject(inner_pos.first, inner_pos.second, BOM_CENTER);
	bo->setExplosionTime(Framework::instance().time());
	BomberGame::instance().getDynamicObject().push_back(*bo);


	auto should_stop = [&](int i,int j, EXPLOSION_DIRECTION ex_direction)->bool {
		if (!validIndex(i, j))
			return true;
		if (BomberGame::instance().getGameObject(i, j).getType() == IRON_WALL)
			return true;
		for (auto& d : BomberGame::instance().getDynamicObject()) {
			if (d.getInnerPos().first == i && d.getInnerPos().second == j && d.destroyable()&&d.isAlive()) {
				if (d.getType() == BOMB)
					d.explode();
				d.kill();
			}
		}
		BomberObject& t = BomberGame::instance().getGameObject(i, j);
		if (t.destroyable())
			t.kill();

		// 设置炸弹冲击波
		Type type = EXPLOSION_V;
		if (ex_direction == HORIZON)
			type = EXPLOSION_H;
		BomberObject* bo = new BomberObject(i, j, type);
		bo->setExplosionTime(Framework::instance().time());
		BomberGame::instance().getDynamicObject().push_back(*bo);
		
		return false;
		};
	auto explode_handler = [&](int dx,int dy, EXPLOSION_DIRECTION ex_direction) {
		for(int delta = 1;delta<=power;delta++)
			if (should_stop(inner_pos.first + dx*delta, inner_pos.second+dy*delta,ex_direction))
				break;
		};
	explode_handler(1, 0, VERTICAL);
	explode_handler(-1, 0, VERTICAL);
	explode_handler(0, 1, HORIZON);
	explode_handler(0, -1, HORIZON);
	BomberGame::bomb_cnt++;
}
void BomberObject::setExplosionTime(unsigned t)
{
	explode_start_time_ = t;
}
bool BomberObject::shouldShockwave() const
{
	
	if (Framework::instance().time() - explode_start_time_ < EXPLODE_TIME)
		return true;
	return false;
}
bool BomberObject::shouldBombCenter() const
{
	if (Framework::instance().time() - explode_start_time_ < BOMBER_CENTER_DURATION)
		return true;
	return false;
}
// 在屏幕的i、j位置绘制物体
BomberObject& BomberObject::operator=(char c) {
	this->type_ = static_cast<Type> (c);
	return *this;
}
