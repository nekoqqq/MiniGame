#pragma once
#include<memory>
#include "../Console/DDS.h"
#include "VisualGame.h"
#include <vector>
#include "GameLib/Framework.h"
#include <utility>

struct DDS;
class BomberGame;
class Bomb;
using GameLib::Framework;
using std::pair;
using std::max;
using std::min;
using std::shared_ptr;
using std::vector;

const int PIXEL_SIZE_ = 16;
const int WIDTH_ = 320 / PIXEL_SIZE_;
const int HEIGHT_ = 240 / PIXEL_SIZE_;

// 游戏中出现的所有物体
class BomberObject {
public:
	enum Type {
		P1_PLAYER, // 1号玩家
		P2_PLAYER, // 2号玩家
		BOMB, // 炸弹
		BOMB_RED, // 增加炸弹威力
		BOM_BLUE, // 增加炸弹范围
		EXPLOSION_H, // 炸弹水平冲击波
		EXPLOSION_V, // 炸弹垂直冲击波
		BOM_CENTER, // 炸弹爆炸时候的中心
		SOFT_WALL, // 可破坏的墙
		ENEMY, // 敌人
		IRON_WALL, // 无法破坏的墙
		GROUND  // 地面
	};
	enum Direction {
		UNKNOWN,
		NORTH,
		EAST,
		SOUTH,
		WEST
	};
	bool isDynamic()const;
	BomberObject();
	BomberObject(int x);
	BomberObject(int x_, int y_, int r_);
	BomberObject(int i, int j, Type type);

	BomberObject& operator=(char c);

	bool isDestroyed()const;
	pair<int, int> softPos()const;
	pair<int, int> bombPos()const;
	void drawAtScreen()const;
	void operator=(int x);
	bool validPos();
	bool validIndex(int i, int j);
	void move(int dx, int dy);

	bool isCollision(BomberObject& o);
	void setCoordinate(double x, double y);
	Type getType()const;
	void setType(Type type);
	void setPutTime(unsigned t);
	void drawAt(int, int)const; // 在屏幕的i、j位置绘制物体

	BomberObject* createBomb();

	pair<int, int> getInnerPos()const; // 获取在网格中的位置

	void kill() {
		expired_ = true;
	}

	bool isAlive()const {
		return !expired_;
	}

	bool destroyAble()const;

	bool shouldExplode()const;
	void explode();


private:
	Type type_;
	shared_ptr<DDS> object_img;
	const int SIZE_PER_IMG = 4;
	const double PLAYER_SPEED = 1; // 速度缩放，每帧移动像素/SPEED_SCALE
	const double ENEMY_SPEED = 1; // 每帧移动x个像素
	
	// 中心点坐标
	double x_;
	double y_;
	double r_;

	Direction direction; // 人物朝向

	unsigned put_time_; // 放置时间
	unsigned explode_start_time_; // 爆炸开始时间
	int power_; // 威力
	bool expired_; // 是否还存活
};