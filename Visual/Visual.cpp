// Visual.cpp : 定义应用程序的入口点。
//

#include "Visual.h"
#include "GameLib/Framework.h"
using namespace std;
using namespace GameLib;
void VisualGame::update() {
	// 上一次各个按键是否被按下
	static bool is_previous_key_on_w = false;
	static bool is_previous_key_on_a = false;
	static bool is_previous_key_on_s = false;
	static bool is_previous_key_on_d = false;

	int direction;
	Framework framework = Framework::instance();

	// 当前按键输入
	bool cur_key_on_w = (framework.isKeyOn('w') || framework.isKeyOn('W'));
	bool cur_key_on_a = (framework.isKeyOn('a') || framework.isKeyOn('A'));
	bool cur_key_on_s = (framework.isKeyOn('s') || framework.isKeyOn('S'));
	bool cur_key_on_d = (framework.isKeyOn('d') || framework.isKeyOn('D'));
	if (!is_previous_key_on_w && cur_key_on_w) 
		direction = 0;
	else if (!is_previous_key_on_a && cur_key_on_a) 
		direction = 1;
	else if (!is_previous_key_on_s && cur_key_on_s) 
		direction = 2;
	else if (!is_previous_key_on_d && cur_key_on_d) 
		direction = 3;
	else
		return;

	is_previous_key_on_w = cur_key_on_w;
	is_previous_key_on_a = cur_key_on_a;
	is_previous_key_on_s = cur_key_on_s;
	is_previous_key_on_d = cur_key_on_d;
	_update_objects(player_pos_, direction);
}
void VisualGame::update(string&){}
void VisualGame::draw() { // 同时向控制台和图形界面输出，控制台是用来debug的
	static int count = 0;
	unsigned* p_vram = Framework::instance().videoMemory();
	unsigned color = 0u;
	int window_width = Framework::instance().width();

	auto draw_cell = [&](int src_x, int src_y, OBJECT object) {
		unsigned* p_img = get_image_data(object);
		int img_width = get_image_width(object);
		int img_height = get_image_height(object);
		// TODO 这里的混合有问题，最终总是画面偏黄
		// 线性混合,z = a*x+(1-a)*y = y + a*(x-y) 
		auto alpha_mix = [&](unsigned src_data,unsigned dst_data) {	
			unsigned src_data_A = (src_data & 0xff000000) >> 24;			
			unsigned src_data_R = (src_data & 0x00ff0000);
			unsigned src_data_G = (src_data & 0x0000ff00);
			unsigned src_data_B = (src_data & 0x000000ff);
	
			unsigned dst_data_A = (dst_data&& 0xff000000) >> 24;
			unsigned dst_data_R = (dst_data & 0x00ff0000);
			unsigned dst_data_G = (dst_data & 0x0000ff00);
			unsigned dst_data_B = (dst_data & 0x000000ff);

			unsigned r = dst_data_R + dst_data_A / 255.f * (src_data_R - dst_data_R);
			unsigned g = dst_data_G + dst_data_A / 255.f * (src_data_G - dst_data_G);
			unsigned b = dst_data_B + dst_data_A / 255.f * (src_data_B - dst_data_B);
			return b | (g & 0x00ff00) | (r & 0xff0000);
		};

		for (int i = 0; i < img_height; i++)
			for (int j = 0; j < img_width; j++) {
				int src_index = (src_x + i) * window_width + src_y + j;
				int dst_index = i * img_width + j;
				unsigned src_data = p_vram[src_index];
				unsigned dst_data = p_img[dst_index];
				unsigned mix_data = alpha_mix(src_data, dst_data);
				p_vram[src_index] = mix_data;

			}
	};
	GameLib::cout << "第" <<count++<< "次更新";
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
	void Framework::update() {
		static bool initialized = false;
		if (!initialized) {
			p_visualGame = new VisualGame();
			p_visualGame->init(MapSource::FILE);
			initialized = true;
			GameLib::cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << GameLib::endl;
		}
		p_visualGame->update();
		p_visualGame->draw();
		if (p_visualGame-> is_finished())
		{
			GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << p_visualGame->steps_ << "." << GameLib::endl;
			requestEnd();
		}
		if (isEndRequested()) {
			delete p_visualGame;
			p_visualGame = nullptr;
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
		}
	}
}
