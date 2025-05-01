//#include "VisualGame.h"
//#include "HSMGame.h"
#include "GameLib/Framework.h"
#include "RootState.h"
#include "StringDrawer.h"
#include <cstdlib>
#include "../Console/DDS.h"
#include "BomberGame.h"
namespace GameLib {
	RootState& g_root_state = RootState::instance();
	int gCounter = 0;
	BomberGame &bg = BomberGame::instance();
	// 框架循环
	void Framework::update() {
		 //结束处理
		GameLib::cout << "第" << ++gCounter << "次更新" << endl;
		Framework framework = Framework::instance();
		if (framework.isKeyTriggered('q') || framework.isKeyTriggered('Q'))
			framework.requestEnd();
		if (framework.isEndRequested()) {
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
		bg.update();
		bg.draw();		
	}
} 