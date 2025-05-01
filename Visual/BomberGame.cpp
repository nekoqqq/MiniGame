#include "BomberGame.h"
#include "BomberObject.h"
#include <vector>
#include <utility>
using std::vector;
using std::pair;

int BomberGame::bomb_cnt = 3;
BomberGame& BomberGame::instance() {
	static BomberGame instance_;
	return instance_;
}
void BomberGame::update() {
	Framework f = Framework::instance();
	int dx = 0, dy = 0;
	if (f.isKeyOn('w')) {
		dx -= 1;
	}
	else if (f.isKeyOn('s')) {
		dx += 1;
	}
	// 现在可以输入斜方向键
	if (f.isKeyOn('a')) {
		dy -= 1;
	}
	else if (f.isKeyOn('d')) {
		dy += 1;
	}

	// 动的物体移动 
	for (int i = 0; i < dynamic_object.size(); i++) {
		BomberObject& bo = dynamic_object[i];
		bo.move(dx, dy);
		if (bo.getType() == PLAYER1P && f.isKeyTriggered(' ') && bomb_cnt>0) {
			BomberObject* new_bomb = bo.createBomb();
			if (new_bomb) {
				bomb_cnt--;
				dynamic_object.push_back(*new_bomb);
			}
		}
	}


	for (auto &b : dynamic_object) {
		if (b.getType() == BomberObject::BOMB && b.shouldExplode()) {
			b.explode();
		}
	}

	vector<BomberObject> new_dyanmic_object;
	for (int i = 0; i < dynamic_object.size(); i++)
		if (dynamic_object[i].isAlive())
			new_dyanmic_object.push_back(dynamic_object[i]);
	dynamic_object = std::move(new_dyanmic_object);
}
BomberObject& BomberGame::getGameObject(int i, int j)
{
	return static_object[i][j];
}
vector<BomberObject>& BomberGame::getGameObjectList(BomberObject::Type type)
{
	vector<BomberObject> res;
	for(int i =0;i<static_object.size();i++)
		for(int j =0;j<static_object[i].size();j++)
			if (static_object[i][j].getType() == type) {
				res.push_back(static_object[i][j]);
			}
	return res;
}
void BomberGame::setGameObject(int i, int j, BomberObject::Type type)
{
	static_object[i][j].setType(type);
}
int BomberGame::getBomberPower() const
{
	return bomb_power;
}
BomberGame::BomberGame() :VisualGame(0, true) {
	mode = PLAYER2P;
	static_object.resize(HEIGHT_);
	for (int i = 0; i < HEIGHT_; i++)
		static_object[i].resize(WIDTH_);

	pair<int, int> p1_pos{ HEIGHT_ - 2,1 };
	pair<int, int > p2_pos{ HEIGHT_ - 2,WIDTH_ - 2 };
	vector<pair<int, int>> reserved_pos{ {13, 2}, { 12,1 } };

	// 墙壁和地面初始化

	for (int i = 0; i < HEIGHT_; i++)
		for (int j = 0; j < WIDTH_; j++) {
			if (i == p1_pos.first && j == p1_pos.second) {
				dynamic_object.push_back({ i,j,BomberObject::P1_PLAYER });
			}
			else if (mode == PLAYER2P && i == p2_pos.first && j == p2_pos.second) {
				dynamic_object.push_back({ i,j,BomberObject::P2_PLAYER });
			}
			else {
				// 保留可以动的位置
				if (pair<int, int>(i, j) == reserved_pos[0] || pair<int, int>(i, j) == reserved_pos[1])
					static_object[i][j] = BomberObject::GROUND;
				else if (i == 0 || i == HEIGHT_ - 1 || j == 0 || j == WIDTH_ - 1) {
					static_object[i][j] = BomberObject::IRON_WALL;
				}
				else if (i % 2 == 0 && j % 2 == 0) {
					static_object[i][j] = BomberObject::IRON_WALL;
				}
				else {
					if (rand() % 100 < SOFT_PCT) // 当前设置1/3比例的可破坏量
						dynamic_object.push_back({ i,j,BomberObject::SOFT_WALL });
				}
			}
			static_object[i][j].setCoordinate(i * PIXEL_SIZE_ + PIXEL_SIZE_ / 2, j * PIXEL_SIZE_ + PIXEL_SIZE_ / 2);
		}

	auto random_generate = [&](int cnt, BomberObject::Type type) {
		while (cnt > 0) {
			int i = rand() % HEIGHT_;
			int j = rand() % WIDTH_;
			if (static_object[i][j].getType() == BomberObject::GROUND) {
				dynamic_object.push_back({ i,j,type });
				cnt--;
			}
		}
		};
	random_generate(ENEMY_CNT, BomberObject::ENEMY);
	random_generate(BOMBER_RED_CNT, BomberObject::BOMB_RED);
	random_generate(BOMBER_BLUE_CNT, BomberObject::BOM_BLUE);

	bomb_cnt = 3;
	bomb_power = 2;
}
vector<BomberObject>& BomberGame::getDynamicObject() {
	return dynamic_object;

}

void BomberGame::draw() {
	// 背景绘制
	for (int i = 0; i < static_object.size(); i++)
		for (int j = 0; j < static_object[i].size(); j++) {
			static_object[i][j].drawAtScreen();
		}
	// 前景绘制
	for(int i =0;i<dynamic_object.size();i++)
		dynamic_object[i].drawAtScreen();



	// 爆炸绘制
}
