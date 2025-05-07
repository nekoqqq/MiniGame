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
#include "GameSound.h"
using namespace GameLib::Input;

namespace GameLib {
	GameContext& g_root_state = GameContext::instance();
	int gCounter = 0;
	// 框架循环
	void Framework::update() {
		Keyboard keyboard = GameLib::Input::Manager::instance().keyboard(); // 放在主线程里面，不能放在外部
		GameLib::cout << "第" << ++gCounter << "次更新" << endl;
		Framework framework = Framework::instance();
		if (keyboard.isTriggered('q') || keyboard.isTriggered('Q'))
			framework.requestEnd();
		if (keyboard.isTriggered('t') || keyboard.isTriggered('T')) // 临时处理加入音乐，不知道为什么播放音乐总是失败
			GameSound::instance().playBGM(GameSound::THEME);
		if (keyboard.isTriggered('y') || keyboard.isTriggered('Y')) // 临时处理加入音乐，不知道为什么播放音乐总是失败
			GameSound::instance().stopBGM();
		if (framework.isEndRequested()) {
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
		g_root_state.update();	
	}
} 