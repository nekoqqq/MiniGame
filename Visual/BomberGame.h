#pragma once
#include "VisualGame.h"
#include "GameLib/Framework.h"
#include "BomberObject.h"
#include <vector>

using GameLib::Framework;
using std::vector;

extern const int PIXCEL_SIZE;
extern const int GRID_WIDHT ;
extern const int GRID_HEIGHT ;


class BomberGame : public VisualGame {
public:
	enum Mode {
		PLAYER1P,
		PLAYER2P,
	};
	BomberGame();
	vector<BomberObject>& getDynamicObject();
	static BomberGame& instance();
	virtual void draw()override;
	void update();
	BomberObject& getGameObject(int i, int j);
	vector<BomberObject>& getGameObjectList(BomberObject::Type type);
	void setGameObject(int i, int j, BomberObject::Type type);
	int getBomberPower()const;
	static int bomb_cnt;
	virtual void loadGame(int stage)override;
	void BomberInit();


	virtual bool is_finished() const override;


private:
	vector<vector<BomberObject>> static_object;
	vector<BomberObject> dynamic_object;
	BomberObject player_p1;
	BomberObject player_p2;
	Mode mode;
	int bomb_power; //  爆炸威力

	const int ENEMY_CNT = 4; // 敌人数量
	const int SOFT_PCT = 20; // 可以破坏的比例
	const int BOMBER_RED_CNT = 3;
	const int BOMBER_BLUE_CNT = 3;


	BomberGame(const BomberGame&) = delete;
	BomberGame& operator=(const BomberGame&) = delete;
};
