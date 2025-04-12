// Visual.cpp : 定义应用程序的入口点。
//

#include "Visual.h"
#include "GameLib/Framework.h"
using namespace std;
using namespace GameLib;
void VisualGame::update(string& input) {
	int direction;
	for (int i = 0; i < input.size(); i++) {
		switch (input[i]) {
			case 'w':
			case 'W':
				direction = 0;
				break;
			case 'a':
			case 'A':
				direction = 1;
				break;
			case 's':
			case 'S':
				direction = 2;
				break;
			case 'd':
			case 'D':
				direction = 3;
				break;
			case 'q':
			case 'Q':
				return;
			default:
				break;
		}
		_update_objects(player_pos_, direction);
	}

}
void VisualGame::draw() { // 同时向控制台和图形界面输出，控制台是用来debug的
	unsigned* p_vram = Framework::instance().videoMemory();
	unsigned color = 0u;
	int window_width = Framework::instance().width();

	auto draw_cell = [&](int src_x,int src_y,OBJECT object) {
		unsigned* p_img = get_image_data(object);
		int img_width = get_image_width(object);
		int img_height = get_image_height(object);
		for (int i = 0; i < img_height; i++)
			for (int j = 0; j < img_width; j++)
				p_vram[(src_x+i) * window_width + src_y+j] = p_img[i * img_width + j];

	};

	for (int i = 0; i < height_; i++, GameLib::cout << endl)
		for (int j = 0; j < width_; j++)
		{
			OBJECT object = static_cast<OBJECT>(grid_[i][j]);
			draw_cell(i*48, j*48, object);
			GameLib::cout << grid_[i][j];
		}
	GameLib::cout << endl;
}


namespace GameLib {
	VisualGame* p_visualGame = nullptr;
	char c;
	string input;
	void Framework::update() {
		static bool initialized = false;
		if (!initialized) {
			p_visualGame = new VisualGame();
			p_visualGame->init(MapSource::FILE);
			initialized = true;
			GameLib::cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << GameLib::endl;
		}
		GameLib::cin >> c;
		input = { c };
		p_visualGame->update(input);
		p_visualGame->draw();
		if (p_visualGame-> is_finished())
		{
			GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << p_visualGame->steps_ << "." << GameLib::endl;
			requestEnd();
		}
		if (isEndRequested() || input == "q" || input == "Q") {
			delete p_visualGame;
			p_visualGame = nullptr;
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
	}
}
