#include "BomberGame.h"
#include "BomberObject.h"
#include <vector>
#include <utility>
using std::vector;
using std::pair;

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
	else if (f.isKeyOn('a')) {
		dy -= 1;
	}
	else if (f.isKeyOn('s')) {
		dx += 1;
	}
	else if (f.isKeyOn('d')) {
		dy += 1;
	}

	// 动的物体移动 
	for (int i = 0; i < HEIGHT_; i++)
		for (int j = 0; j < WIDTH_; j++)
		{
			BomberObject& bo = bomber_object[i][j];
			if (bo.isDynamic() && bo.getType() ==PLAYER1P) {
				bo.move(dx, dy);
			}
		}
}
BomberObject& BomberGame::getGameObject(int i, int j)
{
	return bomber_object[i][j];
}
BomberGame::BomberGame() :VisualGame(0, true) {
	mode = PLAYER2P;
	bomber_object.resize(HEIGHT_);
	for (int i = 0; i < HEIGHT_; i++)
		bomber_object[i].resize(WIDTH_);
	pair<int, int> p1_pos{ HEIGHT_ - 2,1 };
	pair<int, int > p2_pos{ HEIGHT_ - 2,WIDTH_ - 2 };
	vector<pair<int, int>> reserved_pos{ {13, 2}, { 12,1 } };

	// 墙壁和地面初始化
	for (int i = 0; i < HEIGHT_; i++)
		for (int j = 0; j < WIDTH_; j++) {
			if (i == p1_pos.first && j == p1_pos.second) {
				bomber_object[i][j] = BomberObject::P1_PLAYER;
			}
			// 保留可以动的位置
			else  if (pair<int, int>(i, j) == reserved_pos[0] || pair<int, int>(i, j) == reserved_pos[1])
				bomber_object[i][j]=BomberObject::GROUND;
			else if (mode == PLAYER2P && i == p2_pos.first && j == p2_pos.second) {
				bomber_object[i][j] = BomberObject::P2_PLAYER;
			}
			else if (i == 0 || i == HEIGHT_ - 1 || j == 0 || j == WIDTH_ - 1) {
				bomber_object[i][j] = BomberObject::IRON_WALL;
			}
			else if (i % 2 == 0 && j % 2 == 0) {
				bomber_object[i][j] = BomberObject::IRON_WALL;
			}
			else {
				if (rand() % 100 < SOFT_PCT) // 当前设置1/3比例的可破坏量
					bomber_object[i][j] = BomberObject::SOFT_WALL;
			}
			bomber_object[i][j].setCoordinate(i * PIXEL_SIZE_ + PIXEL_SIZE_/2, j * PIXEL_SIZE_ + PIXEL_SIZE_ / 2);
		}

	auto random_generate = [&](int cnt, BomberObject::Type type) {
		while (cnt > 0) {
			int i = rand() % HEIGHT_;
			int j = rand() % WIDTH_;
			if (bomber_object[i][j].getType() == BomberObject::GROUND) {
				bomber_object[i][j].setType(type);
				bomber_object[i][j].setCoordinate(i * PIXEL_SIZE_ + PIXEL_SIZE_ / 2, j * PIXEL_SIZE_ + PIXEL_SIZE_ /2);
				cnt--;
			}
		}
		};
	random_generate(ENEMY_CNT, BomberObject::ENEMY);
	random_generate(BOMBER_RED_CNT, BomberObject::BOMB_RED);
	random_generate(BOMBER_BLUE_CNT, BomberObject::BOM_BLUE);
}
void BomberGame::draw() {
	// 背景绘制
	for (int i = 0; i < bomber_object.size(); i++)
		for (int j = 0; j < bomber_object[i].size(); j++) {
			if(!bomber_object[i][j].isDynamic())
				bomber_object[i][j].drawAtScreen();
		}
	for (int i = 0; i < bomber_object.size(); i++)
		for (int j = 0; j < bomber_object[i].size(); j++) {
			if (bomber_object[i][j].isDynamic())
				bomber_object[i][j].drawAtScreen();
		}

	// 前景绘制
	// 冲击波绘制
}
