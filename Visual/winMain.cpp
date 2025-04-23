//#include "VisualGame.h"
//#include "HSMGame.h"
#include "GameLib/Framework.h"
#include "RootState.h"
#include "StringDrawer.h"
#include <cstdlib>

namespace GameLib {
	RootState* g_root_state = nullptr;
	int gCounter = 0;
	// 框架循环
	void Framework::update() {
		// 结束处理
		GameLib::cout << "第" << ++gCounter << "次更新" << endl;
		if (!g_root_state)
			g_root_state = new RootState();
		g_root_state->update();
		Framework framework = Framework::instance();
		if (framework.isKeyTriggered('q') || framework.isKeyTriggered('Q'))
			framework.requestEnd();

		if (framework.isEndRequested()) {
			DYNAMIC_DEL(g_root_state);
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
	}
}
