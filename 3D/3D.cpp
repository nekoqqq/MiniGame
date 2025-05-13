#include "Math.h"
#include "GameLib/Framework.h"
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
#include "Robot.h"
#include <sstream>
using namespace std;
using namespace GameLib::Input;
const double PI = 3.141592653589793238;
const int WIDTH = 640;
const int HEIGHT = 480;
// 动物
Mecha* gPlayer = nullptr;
Mecha* gEnemy = nullptr;
// 静物
Stage* gStage = nullptr;
// 变换矩阵
Matrix44 gProjectionTransform;
Matrix44 gViewTransform;
// 摄像机
Vector3 gEyePos(50, 50, 50);
Vector3 gTargetPos(0, 0, 0);
Vector3 gEyeUp(0., 1., 0.); // 一般是向上
namespace GameLib {
	bool firstFrame = true;
	void draw_coor(const Vector3&);
	void Framework::update() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			setFrameRate(144);
			firstFrame = false;
			gStage = new Stage();
			gPlayer = new Mecha(Mecha::PLAYER);
			gEnemy = new Mecha(Mecha::ENEMY);
			gProjectionTransform.setProjection(PI / 3, 1.0*WIDTH / HEIGHT, 1, 1000);
		}
		double dx = 0.,dy=0., dz = 0.;
		if (k.isOn('w')) {
			dz = 1.0;
		}
		if (k.isOn('a')) {
			dx = -1.0;
		}
		if (k.isOn('s')) {
			dz = -1.0;
		}
		if (k.isOn('d')) {
			dx = 1.0;
		}
		if (k.isOn(' ')) {
			dy = 1.0;
		}
		if (k.isOn('z')) {
			dy = -1.0;
		}
		gPlayer->move(dx, dy, dz);
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Vector3 player_dir = gPlayer->getWorldDirection();
		gEyePos  = player_dir + Vector3(0,10,-5);
		gTargetPos = player_dir +Vector3(0,0,10);
		gViewTransform.setView(gEyePos, gTargetPos, gEyeUp);
		gStage->draw(gViewTransform, gProjectionTransform);
		gPlayer->draw(gViewTransform,gProjectionTransform);
		gEnemy->draw(gViewTransform,gProjectionTransform);
		Axis::draw(gViewTransform, gProjectionTransform);
		draw_coor(gPlayer->getPos());
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
	}
	void draw_coor(const Vector3& v) {
		ostringstream oss;
		oss << v.x << " " << v.y << " " << v.z << std::endl;;
		Framework::instance().drawDebugString(0, 0, oss.str().c_str());
	}
}
