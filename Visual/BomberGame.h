#pragma once
#include "VisualGame.h"
#include "GameLib/Framework.h"
#include "BomberObject.h"

using GameLib::Framework;

extern const int PIXEL_SIZE_;
extern const int PIXEL_SIZE_ ;
extern const int WIDTH_ ;
extern const int HEIGHT_ ;
extern const int BOMB_CNT;


class BomberGame : public VisualGame {
public:
	enum Mode {
		PLAYER1P,
		PLAYER2P,
	};
	BomberGame();
	static BomberGame& instance();
	virtual void draw()override;
	void update();
	BomberObject& getGameObject(int i, int j);

private:
	std::vector<std::vector<BomberObject>> bomber_object;
	BomberObject player_p1;
	BomberObject player_p2;
	Mode mode;


	const int ENEMY_CNT = 4; // 敌人数量
	const int SOFT_PCT = 20; // 可以破坏的比例
	const int BOMBER_RED_CNT = 3;
	const int BOMBER_BLUE_CNT = 3;


	BomberGame(const BomberGame&) = delete;
	BomberGame& operator=(const BomberGame&) = delete;
};
