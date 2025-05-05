//#include "VisualGame.h"
//#include "HSMGame.h"
#include "GameLib/Framework.h"
#include "RootState.h"
#include "StringDrawer.h"
#include <cstdlib>
#include "../Console/DDS.h"
#include "BomberGame.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Joystick.h"
using namespace GameLib::Input;

namespace GameLib {
	RootState& g_root_state = RootState::instance();
	int gCounter = 0;
	// 框架循环
	void Framework::update() {
		Keyboard keyboard = Manager::instance().keyboard(); // 放在主线程里面，不能放在外部
		GameLib::cout << "第" << ++gCounter << "次更新" << endl;
		Framework framework = Framework::instance();
		if (keyboard.isTriggered('q') || keyboard.isTriggered('Q'))
			framework.requestEnd();
		if (framework.isEndRequested()) {
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
		g_root_state.update();	
	}
} 