#pragma once
#include<memory>
#include "../Console/DDS.h"
#include "VisualGame.h"
#include <vector>
#include "GameLib/Framework.h"
#include <utility>

struct DDS;
class BomberGame;
using GameLib::Framework;
using std::pair;
using std::max;
using std::min;

const int PIXEL_SIZE_ = 16;
const int WIDTH_ = 320 / PIXEL_SIZE_;
const int HEIGHT_ = 240 / PIXEL_SIZE_;
const int BOMB_CNT = 3;

// 游戏中出现的所有物体
class BomberObject { 
public:
	enum Type {
		P1_PLAYER  , // 1号玩家
		P2_PLAYER  , // 2号玩家
		BOMB   , // 炸弹
		BOMB_RED  , // 增加炸弹威力
		BOM_BLUE  , // 增加炸弹范围
		EXPLOSION_H , // 炸弹水平冲击波
		EXPLOSION_V , // 炸弹垂直冲击波
		BOM_CENTER , // 炸弹爆炸时候的中心
		SOFT_WALL , // 可破坏的墙
		ENEMY , // 敌人
		IRON_WALL  , // 无法破坏的墙
		GROUND  // 地面
	};
	bool isDynamic()const;
	BomberObject();
	BomberObject(int x);
	BomberObject(int x_, int y_, int r_);
	bool isDestroyed()const;
	pair<int, int> softPos()const;
	pair<int, int> bombPos()const;
	void drawAtScreen()const;
	void operator=(int x);
	bool validPos();
	bool validIndex(int i,int j);
	void move(int dx, int dy);

	bool isCollision(BomberObject& o);
	void setCoordinate(double x, double y);
	Type getType()const;
	void setType(Type type);
	void drawAt(int, int)const; // 在屏幕的i、j位置绘制物体
	BomberObject& operator=(char c);

private:
	Type type_;
	std::unique_ptr<DDS> object_img;
	const int SIZE_PER_IMG = 4;
	const double SPEED_SCALE = 1; // 速度缩放，每帧移动像素/SPEED_SCALE
	
	// 中心点坐标
	double x_;
	double y_;
	double r_;



	bool is_destroyed_; // 是否被炸弹破坏过
	int bomb_cnt;
	bool has_bomb;
};


