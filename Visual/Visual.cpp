// Visual.cpp : 定义应用程序的入口点。
//

#include "Visual.h"
#include "GameLib/Framework.h"

using namespace GameLib;
void VisualGame::update(string& string) {

}
void VisualGame::draw() {
	unsigned* p_vram = Framework::instance().videoMemory();
	unsigned color = 0u;
	int window_width = Framework::instance().width();
	for (int i = 0; i < height_; i++, GameLib::cout << endl)
		for (int j = 0; j < width_; j++)
		{
			switch (grid_[i][j]) {
			case BOX:
				color = 0xff0000; // 红色
				break;
			case BOX_READY:
				color = 0xff0080; // 红紫
			case PLAYER:
				color = 0x0000ff; // 蓝色
				break;
			case PLAYER_HIT:
				color = 0x0080ff; // 浅蓝色
			case TARGET:
				color = 0x00ff00; // 绿色
				break;
			case BOUNDARY:
				color = 0xff6699; // 某种洋色
				break;
			case BLANK:
				color = 0xffffff; // 白色
				break;
			}
			
			int scale = 32;
			for(int k=0;k< scale;k++)
				for (int w = 0; w < scale; w++)
				{
					p_vram[(i* window_width +j)* scale + k* window_width +w] = color;
					
				}
			GameLib::cout << grid_[i][j];
		}
	GameLib::cout << endl;
}


namespace GameLib {
	VisualGame* p_visualGame = nullptr;
	int f(int);
	void Framework::update() {
		static bool initialized = false;
		if (!initialized) {
			p_visualGame = new VisualGame();
			p_visualGame->init(MapSource::FILE);
			initialized = true;
		}
		p_visualGame->draw();
	}
}
