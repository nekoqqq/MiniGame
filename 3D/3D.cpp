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
Mecha* player = nullptr;
Mecha* enemy = nullptr;

// 静物
Stage* stage = nullptr;



// 变换矩阵
Matrix44 gProjectionTransform;
Matrix44 gViewTransform;

// 摄像机
Vector3 eye_pos(50, 50, 50);
Vector3 target_pos(0, 0, 0);
Vector3 up(0., 1., 0.); // 一般是向上


namespace GameLib {
	bool firstFrame = true;
	void Framework::update() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();

		


		if (firstFrame) {
			firstFrame = false;
			stage = new Stage();
			player = new Mecha(Mecha::PLAYER);
			enemy = new Mecha(Mecha::ENEMY);

			gProjectionTransform.setProjection(PI / 3, 1.0*WIDTH / HEIGHT, 1, 1000);
		}
		if (k.isOn('w')) {
			target_pos.z += 1;
		}
		if (k.isOn('a')) {
			target_pos.x -= 1;
		}
		if (k.isOn('s')) {
			target_pos.z -= 1;
		}
		if (k.isOn('d')) {
			target_pos.x += 1;
		}
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();


		target_pos.x += m.velocityX();
		target_pos.y += m.velocityY();
		target_pos.z += m.wheel();

		gViewTransform.setView(eye_pos, target_pos, up);

		ostringstream oss;
		oss << target_pos.x << " " << target_pos.y << " " << target_pos.z;
		Framework::instance().drawDebugString(0, 0,oss.str().c_str());
		oss.str("");

		player->update();
		enemy->update();

		stage->draw(gViewTransform, gProjectionTransform);
		player->draw(gViewTransform,gProjectionTransform);
		enemy->draw(gViewTransform,gProjectionTransform);



	}
}
