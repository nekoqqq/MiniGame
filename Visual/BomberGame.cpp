#include "BomberGame.h"
#include "BomberObject.h"
#include <vector>
#include <utility>
using std::vector;
using std::pair;
using std::remove_if;

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

	// 动物移动 
	for (auto& o : dynamic_object)
		if (o.movable())
			o.move(dx, dy);


	// 放置炸弹
	for (auto& o : dynamic_object)
		if (o.getType() == PLAYER1P && f.isKeyTriggered(' ') && bomb_cnt > 0) {
			BomberObject* new_bomb = o.createBomb();
			if (new_bomb) {
				bomb_cnt--;
				dynamic_object.push_back(*new_bomb);
			}
		}
		else if (o.getType() == PLAYER2P && f.isKeyTriggered('x') && bomb_cnt > 0) {

		}

	// 爆炸处理
	for (auto& o : dynamic_object){
		if (o.explodable() && o.shouldExplode()) 
			o.explode();
	}

	// 爆炸中心处理
	for (auto& o : dynamic_object) {
		if (o.getType() == BomberObject::BOM_CENTER && !o.shouldBombCenter())
			o.kill();
	}


	// 冲击波处理
	for (auto& o : dynamic_object) {
		if (o.shockwavable() && !o.shouldShockwave())
			o.kill();
	}

	// 清除掉无效的物体
	dynamic_object.erase(remove_if(dynamic_object.begin(), dynamic_object.end(), [](const auto& o) {return !o.isAlive(); }),dynamic_object.end());
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
void BomberGame::loadGame(int stage)
{
	BomberInit();
	mode = PLAYER1P;
	static_object.resize(GRID_HEIGHT);
	for (int i = 0; i < GRID_HEIGHT; i++)
		static_object[i].resize(GRID_WIDHT);

	pair<int, int> p1_pos{ GRID_HEIGHT - 2,1 };
	pair<int, int > p2_pos{ GRID_HEIGHT - 2,GRID_WIDHT - 2 };
	vector<pair<int, int>> reserved_pos{ {13, 2}, { 12,1 } };

	// 墙壁和地面初始化

	for (int i = 0; i < GRID_HEIGHT; i++)
		for (int j = 0; j < GRID_WIDHT; j++) {
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
				else if (i == 0 || i == GRID_HEIGHT - 1 || j == 0 || j == GRID_WIDHT - 1) {
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
			static_object[i][j].setCoordinate(i * PIXCEL_SIZE + PIXCEL_SIZE / 2, j * PIXCEL_SIZE + PIXCEL_SIZE / 2);
		}

	auto random_generate = [&](int cnt, BomberObject::Type type) {
		while (cnt > 0) {
			int i = rand() % GRID_HEIGHT;
			int j = rand() % GRID_WIDHT;
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
void BomberGame::BomberInit()
{
	static_object.clear();
	dynamic_object.clear();

}
bool BomberGame::is_finished() const
{
	for (auto& d : dynamic_object)
		if (d.getType() == BomberObject::ENEMY)
			return false;
	return true;
}
BomberGame::BomberGame() :VisualGame(0, true) {
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
	for (auto& o : dynamic_object)
		if (!o.shockwavable())
			o.drawAtScreen();

	// 爆炸中心绘制
	for (auto& o : dynamic_object)
		if (o.getType()==BomberObject::BOM_CENTER)
			o.drawAtScreen();


	// 爆炸绘制
	for (auto& o : dynamic_object)
		if (o.shockwavable())
			o.drawAtScreen();
}
