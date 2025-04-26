#pragma once
#include<memory>
#include "../Console/DDS.h"
#include "../Console/GameObject.h"
#include "VisualGame.h"
#include <vector>
#include "GameLib/Framework.h"
#include <utility>
struct DDS;
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
	BomberObject(int x):is_destroyed_(false),bomb_cnt(BOMB_CNT),has_bomb(false) {
		this->type_ = static_cast<Type> (x);
	}
	bool isDestroyed()const {
		return is_destroyed_;
	}

	pair<int, int> softPos()const {
		return { 8/4,8%4 };
	}
	pair<int, int> bombPos()const {
		return { 0,2 };
	}

	void drawAtScreen()const {
		int i = getType() / 4;
		int j = getType() % 4;
		pair<int, int> soft_pos = softPos();
		pair<int, int> bomb_pos = bombPos();
		if (getType() == BOMB_RED || getType() == BOM_BLUE) {
			if (isDestroyed())
				object_img->drawAtScreen(i, j, (int)x_, (int)y_);
			else
				object_img->drawAtScreen(soft_pos.first, soft_pos.second,(int)x_,(int)y_);
		}
		else
			object_img->drawAtScreen(i, j, (int)x_, (int)y_);
	}
	void operator=(int x) {
		this->type_ = static_cast<Type>(x);

	}

	void move(int dx, int dy) {
		// 敌人随机移动
		if (getType() == ENEMY) {
			x_ += dx * rand() % 2 / SPEED_SCALE;
			y_ += dy * rand() % 2 / SPEED_SCALE;
		}
		else {
			x_ += dx / SPEED_SCALE;
			y_ += dy / SPEED_SCALE;
		}
	}

	void setBomb() {
		if (bomb_cnt-- > 0) {
			has_bomb = true;
		}
	}

	bool isBombSet()const {
		return true;
	}

	bool hasBomb()const {
		return has_bomb;
	}

	void setCoordinate(double x, double y) {
		x_ = x;
		y_ = y;
	}
	Type getType()const;
	void setType(Type type) {
		type_ = type;
	}
	void drawAt(int, int)const; // 在屏幕的i、j位置绘制物体
	BomberObject& operator=(char c) {
		this->type_ = static_cast<Type> (c);
		return *this;
	}

private:
	Type type_;
	std::unique_ptr<DDS> object_img;
	const int SIZE_PER_IMG = 4;
	const double SPEED_SCALE = 0.2; // 速度缩放，每帧移动像素/SPEED_SCALE
	
	// 坐标
	double x_;
	double y_;

	bool is_destroyed_; // 是否被炸弹破坏过

	int bomb_cnt;
	bool has_bomb;

};


class BomberGame: public VisualGame{
public: 
	enum Mode {
		PLAYER1P,
		PLAYER2P,
	};
	BomberGame();
	
	virtual void draw()override;
	void update() {
		Framework f = Framework::instance();
		int dx = 0, dy = 0;
		if (f.isKeyTriggered('w')) {
			dx -= 1;
		}
		else if (f.isKeyTriggered('a')) {
			dy -= 1;
		}
		else if (f.isKeyTriggered('s')) {
			dx += 1;
		}
		else if (f.isKeyTriggered('d')) {
			dy += 1;
		}

		// 动的物体移动 
		for(int i =0;i<HEIGHT_;i++)
			for (int j = 0; j < WIDTH_; j++)
			{
				BomberObject& bo = bomber_object[i][j];
				if (bo.isDynamic()) {
					bo.move(dx, dy);
				}
			}


	}

private:
	std::vector<std::vector<BomberObject>> bomber_object;
	BomberObject player_p1;
	BomberObject player_p2;
	Mode mode;
	

	const int ENEMY_CNT = 4; // 敌人数量
	const int SOFT_PCT = 20; // 可以破坏的比例
	const int BOMBER_RED_CNT = 3;
	const int BOMBER_BLUE_CNT = 3;
};