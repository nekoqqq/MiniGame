#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
#include "Library/Model.h"
#include "Library/Graph.h"
#include "Library/Camera.h"
#include "Library/Resource.h"
#include "Library/Mecha.h"
using namespace std;
using namespace GameLib::Input;
const double PI = 3.141592653589793238;
const int WIDTH = 640;
const int HEIGHT = 480;

// 游戏
const int MAX_TIME = 120 * FRAMES; // 最大对局时间
// 动物
Model* gPlayer;
Model* gEnemy;
vector<Model*>gEnemys;
const int gEnemyCnt = 1;
// 静物
Model* gStage;
Model* gWall;
Model* gAxis;
// 变换矩阵
Matrix44 gProjectionTransform;
Matrix44 gViewTransform;
// 摄像机
Camera* gCamera;
Vector3 gEyePos(0, 0, 1);
Vector3 gTargetPos(0, 0, 0);
Vector3 gEyeUp(0., 1., 0.); // 一般是向上
const double fov_y = PI / 3;
const double aspec_ratio = 1.0 * WIDTH / HEIGHT;
const double near = 1.0;
const double far = 10000.0;
int gCounter = 0;
// 资源类
Resource* gResource;
namespace GameLib {
	bool firstFrame = true;
	void deleteAll() {
		SAFE_DELETE(gPlayer);
		SAFE_DELETE(gEnemy);
		for (int i = 0; i < gEnemyCnt; i++)
			SAFE_DELETE(gEnemys[i]);
		gEnemys.clear();
		SAFE_DELETE(gStage);
		SAFE_DELETE(gWall);
	}
	void Framework::update() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			setFrameRate(FRAMES);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gPlayer = gResource->createModel(Model::PLAYER, CollisionModel::SPHERE, "player");
			for (int i = 0; i < MAX_MISSLES; i++) {
				Model * missle = gResource->createModel(Model::MISSLE, CollisionModel::SPHERE, "missle");
				dynamic_cast<Mecha*>(gPlayer)->addMissle(*missle);
			}
			gEnemy = gResource->createModel(Model::ENEMY, CollisionModel::SPHERE, string("enemy"));
			for (int i= 0;i<MAX_MISSLES;i++)
			{
				Model* missle = gResource->createModel(Model::MISSLE, CollisionModel::SPHERE, "missle");
				dynamic_cast<Mecha*>(gEnemy)->addMissle(*missle);
			}
			for (int i = 0; i < gEnemyCnt; i++) {
				gEnemys.push_back(gResource->createModel(Model::ENEMY, CollisionModel::SPHERE, string("enemy_") + to_string(i+1)));
			}
			dynamic_cast<Mecha*>(gPlayer)->addEnemy(gEnemy);
			dynamic_cast<Mecha*>(gEnemy)->addEnemy(gPlayer);
			gStage = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "stage");
			gWall = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "wall");
			gAxis = gResource->createModel(Model::AXIS, CollisionModel::SPHERE, "axis");
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);
			// 设置碰撞物体
			gPlayer->setCollisionModels(gEnemys);
			gPlayer->addCollisionModel(gEnemy);
			gPlayer->addCollisionModel(gStage);
			gPlayer->addCollisionModel(gWall);
			gEnemy->addCollisionModel(gPlayer);
			gEnemy->addCollisionModel(gStage);
			gEnemy->addCollisionModel(gWall);
			++gCounter;
		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Matrix44 vr = gCamera->getViewRotation();
		gPlayer->update(vr); // 玩家和敌人共用一套逻辑
		gEnemy->update(vr);
		gCamera->update(gPlayer);

		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		gStage->draw(pv);
		gWall->draw(pv);
		gAxis->draw(pv);
		gPlayer->draw(pv);
		gEnemy->draw(pv);
		for (int i = 0; i < gEnemyCnt; i++) {
			gEnemys[i]->draw(pv);
		}
		if (!dynamic_cast<Mecha*>(gEnemy)->isAlive() || ++gCounter>=MAX_TIME) {
			drawDebugString(1, 0, "Game Over!");
			sleep(1000);
			deleteAll();
			firstFrame = true;
			gCounter = 0;
		}
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
		if (isEndRequested()) {
			deleteAll();
		}
		gCounter++;
	}
}