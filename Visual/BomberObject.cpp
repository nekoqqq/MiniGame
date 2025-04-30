#include "../Console/GameObject.h"
#include "VisualGame.h"
#include <vector>
#include "../Console/DDS.h"
#include "BomberObject.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include "GameLib/Framework.h"
#include "BomberGame.h"

using std::pair;
using std::vector;

bool BomberObject::isDynamic() const {
	bool res = false;
	switch (type_)
	{
	case BomberObject::P1_PLAYER:
	case BomberObject::P2_PLAYER:
	case BomberObject::ENEMY:
		res = true;
		break;
	case BomberObject::BOMB:
		break;
	case BomberObject::BOMB_RED:
		break;
	case BomberObject::BOM_BLUE:
		break;
	case BomberObject::EXPLOSION_H:
		break;
	case BomberObject::EXPLOSION_V:
		break;
	case BomberObject::BOM_CENTER:
		break;
	case BomberObject::SOFT_WALL:
		break;
	case BomberObject::IRON_WALL:
		break;
	case BomberObject::GROUND:
		break;
	}
	return res;
}

BomberObject::BomberObject() {
	object_img = std::make_unique<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\bomber.dds");
	type_ = GROUND;
	is_destroyed_ = false;
}
BomberObject::BomberObject(int x) :is_destroyed_(false), bomb_cnt(BOMB_CNT), has_bomb(false), r_(PIXEL_SIZE_ / 2) {
	this->type_ = static_cast<Type> (x);
}
BomberObject::BomberObject(int x_, int y_, int r_) {
	this->x_ = x_;
	this->y_ = y_;
	this->r_ = r_;
}
bool BomberObject::isDestroyed() const {
	return is_destroyed_;
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
		if (1||isDestroyed())
			object_img->drawAtScreen(i, j, (int)(x_), (int)(y_));
		else
			object_img->drawAtScreen(soft_pos.first, soft_pos.second, (int)(x_-r_), (int)(y_-r_));
	}
	else
		object_img->drawAtScreen(i, j, (int)(x_-r_), (int)(y_-r_));
}
void BomberObject::operator=(int x) {
	this->type_ = static_cast<Type>(x);
}
bool BomberObject::validPos() {
	return 0 <= (x_-r_) && (x_ - r_) < HEIGHT_ * PIXEL_SIZE_ && 0 <= (y_ - r_) && (y_ - r_) < WIDTH_ * PIXEL_SIZE_;
}
bool BomberObject::validIndex(int i,int j) {
	return 0 <= i && i < HEIGHT_ && 0 <= j && j < WIDTH_;
}
void BomberObject::move(int dx, int dy) {
	GameLib::cout << x_ << " " << y_ << GameLib::endl;
	int raw_i = (x_ - r_) / 16;
	int raw_j = (y_ - r_) / 16;

	if (getType() == ENEMY) { // 敌人随机移动
		x_ += Framework::instance().getRandom() % 2 / SPEED_SCALE;
		y_ += Framework::instance().getRandom() % 2 / SPEED_SCALE;
	}
	else {
		x_ += dx / SPEED_SCALE;
		y_ += dy / SPEED_SCALE;
	}
	x_ = min(max(0., x_), (HEIGHT_ - 1.) * PIXEL_SIZE_);
	y_ = min(max(0., y_), (WIDTH_ - 1.) * PIXEL_SIZE_);



	// TODO 现在检查的是周围9个的网格，可以优化到只检查4个网格
	for (int i = raw_i -1; i < raw_i +2; i++)
		for (int j = raw_j -1 ; j < raw_j + 2; j++) {
			BomberObject& t = BomberGame::instance().getGameObject(i, j);
			if (t.getType() == IRON_WALL && isCollision(t)) {
				x_ -= dx / SPEED_SCALE;
				y_ -= dy / SPEED_SCALE;
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
	r_ = PIXEL_SIZE_ / 2;

}
BomberObject::Type BomberObject::getType()const {
	return type_;
}
void BomberObject::setType(Type type) {
	type_ = type;
}
void BomberObject::drawAt(int screen_i,int screen_j)const {
    int i = getType() / 4;
    int j = getType() % 4;
	object_img->drawFrom(i, j, screen_i, screen_j);
}
// 在屏幕的i、j位置绘制物体
BomberObject& BomberObject::operator=(char c) {
	this->type_ = static_cast<Type> (c);
	return *this;
}


