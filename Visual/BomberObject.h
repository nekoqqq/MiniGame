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

const int PIXEL_SIZE = 16;
const int GRID_WIDHT = 320 / PIXEL_SIZE;
const int GRID_HEIGHT = 240 / PIXEL_SIZE;

const int SIZE_PER_IMG = 4;
const double PLAYER_SPEED = 1.; // 玩家移动速度，每帧移动像素*SPEED_SCALE
const double ENEMY_SPEED = 1.; //  敌人移动速度，每帧移动x个像素

const int EXPLODE_TIME = 3000; // 爆炸波显示时间（ms）
const int BOMBER_CENTER_DURATION = 1000; // 爆炸中心持续时间

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
	// 根据中心爆炸的位置决定爆炸朝向
	enum EXPLOSION_DIRECTION {
		HORIZON,
		VERTICAL
	};
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

	BomberObject* createBomb();

	pair<int, int> getInnerPos()const; // 获取在网格中的位置

	void kill();

	bool isAlive()const;


	bool movable()const; // 可以移动的物体
	bool destroyable()const; // 可以被炸弹毁灭的物体
	bool playerable()const; // 是否是玩家角色
	bool shockwavable()const; // 是否是可以冲击波的对象
	bool explodable() const; // 是否可以自发爆炸的物品

	// 爆炸处理
	bool shouldExplode()const;
	void explode();

	void setExplosionTime(unsigned t);
	bool shouldShockwave()const; // 冲击波是否结束是否结束

	bool shouldBombCenter()const; // 是否显示爆炸中心

private:
	Type type_; // 类型
	shared_ptr<DDS> obj_img_; // 存储的图片

	// 中心点坐标
	double x_;
	double y_;
	double r_;

	Direction direction; // 人物朝向

	int bomb_cnt; // 炸弹数量
	Type bomb_owner; // 炸弹的设置人

	unsigned put_time_; // 放置时间
	unsigned explode_start_time_; // 爆炸开始时间
	int power_; // 威力
	bool expired_; // 是否还存活

};