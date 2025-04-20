#include "Visual.h"
#include "GameLib/Framework.h"

namespace GameLib {
	VisualGame* gVisualGame = nullptr;
	GameState gGameState;
	unsigned gLoadingStartTime = 0;
	unsigned gEndingStartTime = 0;
	int gStage = 0;

	// 主循环
	void mainLoop() { // 主游戏循环
		static const int FPS = 60; // 16.66 ms 每一Frame
		static unsigned previous_time[FPS]{}; // 前一次的时间戳
		static int counter = 0; // 游戏循环次数

		Framework framework = Framework::instance();

		GameLib::cout << "第" << ++counter << "次更新" << endl;
		gVisualGame->set_elapsed_time(framework.time() - previous_time[FPS - 1]); // 这里最好睡眠一下
		gVisualGame->update();
		gVisualGame->draw();
		if (gVisualGame->is_finished())
		{
			GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << gVisualGame->steps_ << "." << GameLib::endl;
			gGameState = GameState::ENDING;
			gEndingStartTime = framework.time();
			SAFE_DELETE(gVisualGame);
		}

		if (framework.isKeyTriggered('m') || framework.isKeyTriggered('M')) {
			gGameState = GameState::MENU;
		}

		GameLib::cout << "当前FPS耗时: " << framework.time() - previous_time[FPS - 1] << "ms" << GameLib::endl;
		int interval = framework.time() - previous_time[0];
		if (counter % FPS == 0)
			GameLib::cout << "实际FPS: " << 1000 * FPS / interval << GameLib::endl;

		// 补FPS
		while (framework.time() - previous_time[FPS - 1] < 1.0 / FPS)
			framework.sleep(1);
		for (int i = 0; i < FPS - 1; i++)
			previous_time[i] = previous_time[i + 1];
		previous_time[FPS - 1] = framework.time();
	}

	// 主题循环
	void themeLoop() {
		static DDS* theme_img = nullptr;
		if (!theme_img)
			theme_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\main_theme.dds");
		gVisualGame->drawCell(0,0,*theme_img);
		if (Framework::instance().isKeyTriggered(' ')) // 替换成空格键开始
		{
			gGameState = GameState::SELECTION;
			SAFE_DELETE(theme_img); // 这里如果只用了delete theme_img，那么只是所指向的空间被释放了，但是theme_img这个指针不为空，因此上面的判断仍然可以通过，这也是为什么还需要让theme_img为nullptr
		}
	}

	void selectionLoop() {
		static DDS* selection_img = nullptr;
		if (!selection_img)
			selection_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\selection.dds");
		gVisualGame->drawCell(0,0,*selection_img);
		Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
		for (char c : "123456789") {
			if (f.isKeyTriggered(c)) {
				gStage = c - '0';
				gGameState = GameState::LOADING;
				gLoadingStartTime = f.time();
				SAFE_DELETE(selection_img);
			}
		}
	}

	void loadingLoop() {
		static DDS* loading_img = nullptr;
		if (!loading_img)
			loading_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\loading.dds");
		gVisualGame->drawCell(0, 0, *loading_img);
		Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？

		if (f.time() - gLoadingStartTime > 1000) { // 读取时间1ms
			gGameState = GameState::GAME;
			SAFE_DELETE(loading_img);

			// 在这里实际初始化游戏
			bool var_fps = true;
			if (!gVisualGame) {
				gVisualGame = new VisualGame(gStage, var_fps);
				
			}
			else {
				SAFE_DELETE(gVisualGame);
				gVisualGame = new VisualGame(gStage, var_fps);
			}
			GameLib::cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << GameLib::endl;
		}
	}

	void menuLoop() {
		static DDS* menu_img = nullptr;
		if (!menu_img)
			menu_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\menu.dds");
		gVisualGame->drawCell(0, 0, *menu_img);
		Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
		if (f.isKeyTriggered('1')) { // 重置
			gVisualGame->reset();
			gGameState = GameState::GAME;
			SAFE_DELETE(menu_img);
		}
		else if (f.isKeyTriggered('2')) { // 选关
			gGameState = GameState::SELECTION;
			SAFE_DELETE(menu_img);
		}
		else if (f.isKeyTriggered('3')) { // 回到主题
			gGameState = GameState::THEME;
			SAFE_DELETE(menu_img);
		}
		else if (f.isKeyTriggered('4')) { // 继续
			gGameState = GameState::GAME;
			SAFE_DELETE(menu_img);
		}
	}

	void endingLoop() {
		static DDS* ending_img = nullptr;
		if (!ending_img)
			ending_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\ending.dds");
		gVisualGame->drawCell(0, 0, *ending_img);
		if (Framework::instance().time() - gEndingStartTime > 2000) { // 显示2秒钟
			gGameState = GameState::THEME;
			SAFE_DELETE(ending_img);
		}
	}

	// 框架循环
	void Framework::update() {
		switch (gGameState) {
		case GameState::THEME:
			themeLoop();
			break;
		case GameState::SELECTION:
			selectionLoop();
			break;
		case GameState::LOADING:
			loadingLoop();
			break;
		case GameState::GAME:
			mainLoop();
			break;
		case GameState::MENU:
			menuLoop();
			break;
		case GameState::ENDING:
			endingLoop();
			break;
		}

		// 结束处理
		Framework framework = Framework::instance();
		if (framework.isKeyTriggered('q') || framework.isKeyTriggered('Q'))
			framework.requestEnd();

		if (framework.isEndRequested()) {
			if (gVisualGame)
				SAFE_DELETE(gVisualGame);

			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
	}
}
