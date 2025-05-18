#include "GameLib/Framework.h"
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
#include "Library/Robot.h"
#include <sstream>
using namespace std;
using namespace GameLib::Input;
const double PI = 3.141592653589793238;
const int WIDTH = 640;
const int HEIGHT = 480;
// 动物
Model* gPlayer;
vector<Model*>gEnemys;
Model* gEnemy;
// 静物
Model* gStage;
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
// 资源类
Resource* gResource;
namespace GameLib {
	bool firstFrame = true;
	void draw_coor(const Vector3&);
	void Framework::update() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			setFrameRate(144);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gPlayer = gResource->createModel(Model::PLAYER, "player");

			gEnemy = gResource->createModel(Model::ENEMY, "enemy");
			gStage = gResource->createModel(Model::STAGE, "stage");
			gAxis = gResource->createModel(Model::AXIS, "axis");
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);
			// 设置碰撞物体
			gPlayer->setCollisionModels({gStage,gEnemy});
		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Matrix44 vr = gCamera->getViewRotation();
		gPlayer->update(vr);
		gCamera->update(gPlayer);

		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		gStage->draw(pv);
		gAxis->draw(pv);
		gPlayer->draw(pv);
		gEnemy->draw(pv);
		draw_coor(gTargetPos);

		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();

	}
	void draw_coor(const Vector3& v) {
		ostringstream oss;
		oss << v.x << " " << v.y << " " << v.z << std::endl;;
		Framework::instance().drawDebugString(0, 0, oss.str().c_str());
	}
}