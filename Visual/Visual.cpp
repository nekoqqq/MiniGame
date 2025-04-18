// Visual.cpp : 定义应用程序的入口点。
//

#include "Visual.h"
#include "GameLib/Framework.h"
using namespace std;
using namespace GameLib;
void VisualGame::update() {
	if (move_count == 48) {
		move_count = 0;
		for(int i =0;i<height_;i++)
			for (int j = 0; j < width_; j++) {
				grid_obj[i][j].set_move(0,0);
			}
		return;
	}

	if (move_count > 0) {
		move_count++;
		return;
	}

	int direction=5;
	Framework framework = Framework::instance();

	// 当前按键输入
	bool cur_key_on_w = (framework.isKeyTriggered('w') || framework.isKeyTriggered('W'));
	bool cur_key_on_a = (framework.isKeyTriggered('a') || framework.isKeyTriggered('A'));
	bool cur_key_on_s = (framework.isKeyTriggered('s') || framework.isKeyTriggered('S'));
	bool cur_key_on_d = (framework.isKeyTriggered('d') || framework.isKeyTriggered('D'));
	bool cur_key_on_esc = framework.isKeyTriggered('b');
	if (cur_key_on_w)
		direction = 0;
	else if (cur_key_on_a)
		direction = 1;
	else if (cur_key_on_s)
		direction = 2;
	else if (cur_key_on_d)
		direction = 3;

	// 这里不可以直接return,否则previous_key无法置为当前的输入,就无法响应连续的同一个按键的输入
	_update_objects(direction);
}
void VisualGame::update(string&){}
void VisualGame::update(int t) {
	// 上一次各个按键是否被按下
	static bool previous_key_on_w = false;
	static bool previous_key_on_a = false;
	static bool previous_key_on_s = false;
	static bool previous_key_on_d = false;

	if (var_move_count >= MAX_VAR_MOVE_COUNT) { 
		var_move_count = 0;
		for (int i = 0; i < height_; i++)
			for (int j = 0; j < width_; j++) {
				grid_obj[i][j].set_move(0, 0);
			}
	}

	if (var_move_count > 0) {
		var_move_count = min(var_move_count+t,MAX_VAR_MOVE_COUNT); // 后续需要用MAX_VAR_MOVE_COUNT减去相应的值，因此需要这里做一下处理
		return;
	}

	int direction = 5;
	Framework framework = Framework::instance();

	// 当前按键输入
	bool cur_key_on_w = (framework.isKeyOn('w') || framework.isKeyOn('W'));
	bool cur_key_on_a = (framework.isKeyOn('a') || framework.isKeyOn('A'));
	bool cur_key_on_s = (framework.isKeyOn('s') || framework.isKeyOn('S'));
	bool cur_key_on_d = (framework.isKeyOn('d') || framework.isKeyOn('D'));
	if (!previous_key_on_w && cur_key_on_w)
		direction = 0;
	else if (!previous_key_on_a && cur_key_on_a)
		direction = 1;
	else if (!previous_key_on_s && cur_key_on_s)
		direction = 2;
	else if (!previous_key_on_d && cur_key_on_d)
		direction = 3;
	// 这里不可以直接return,否则previous_key无法置为当前的输入,就无法响应连续的同一个按键的输入

	previous_key_on_w = cur_key_on_w;
	previous_key_on_a = cur_key_on_a;
	previous_key_on_s = cur_key_on_s;
	previous_key_on_d = cur_key_on_d;
	_update_objects(direction);
}
void VisualGame::draw() { // 同时向控制台和图形界面输出，控制台是用来debug的
	// 先绘制背景
	for(int i =0;i<height_;i++)
		for (int j = 0; j < width_; j++) {
			if(grid_obj[i][j]==GameObject::BLANK || grid_obj[i][j]==GameObject::BOUNDARY||grid_obj[i][j]==GameObject::TARGET)
				drawCell(i * 48, j * 48, static_cast<DDS&>(grid_obj[i][j]));
		}


	// 再绘制前景，用于修复从下往上，因为先在上面的格子绘制了人物，后在下面的给子绘制了背景，导致人物的下半身没有了的问题
	// 可变和固定的计算方式不一样，固定是每Frame移动一个固定的像素，而可变的是根据每个Frame的实际消耗时间，移动相应的距离
	if (var_fps) {
		for (int i = 0; i < height_; i++)
			for (int j = 0; j < width_; j++)
			{
				GameObject& go = grid_obj[i][j];
				if (!(go == GameObject::BLANK || go == GameObject::BOUNDARY || go == GameObject::TARGET)) // 玩家或者箱子移动
				{
					int move_dx = go.get_move().first, move_dy = go.get_move().second;
					drawCell(i * 48 - 48*(MAX_VAR_MOVE_COUNT - var_move_count)* move_dx / MAX_VAR_MOVE_COUNT, j * 48 - 48*(MAX_VAR_MOVE_COUNT - var_move_count) * move_dy / MAX_VAR_MOVE_COUNT, static_cast<DDS&>(go));
				}
			}
	}
	else {
		for (int i = 0; i < height_; i++)
			for (int j = 0; j < width_; j++)
			{
				GameObject& go = grid_obj[i][j];
				if (!(go == GameObject::BLANK || go == GameObject::BOUNDARY || go == GameObject::TARGET)) // 玩家或者箱子移动
				{
					int move_dx = go.get_move().first, move_dy = go.get_move().second;
					drawCell(i * 48 - (48 - move_count) * move_dx, j * 48 - (48 - move_count) * move_dy, static_cast<DDS&>(go));
				}
			}
	}
	
	// STD debug out
	for (int i = 0; i < height_; i++, GameLib::cout << endl)
		for (int j = 0; j < width_; j++) {
			GameLib::cout << static_cast<char>(grid_obj[i][j]);
		}
	GameLib::cout << endl;
}
void VisualGame::drawFPS() {
	// TODO 绘制文字那一章再增加实现
}

void VisualGame::drawCell(int src_x, int src_y, DDS &dds)
{
	unsigned* p_vram = Framework::instance().videoMemory();
	unsigned color = 0u;
	int window_width = Framework::instance().width();
	unsigned* p_img = dds.get_image_data();
	int img_width = dds.get_image_width();
	int img_height = dds.get_image_height();
	// TODO 这里的混合有问题，最终总是画面偏黄
	// 线性混合,z = a*x+(1-a)*y = y + a*(x-y) 
	auto alpha_mix = [&](unsigned src_data, unsigned dst_data) {
		unsigned src_data_A = (src_data & 0xff000000) >> 24;
		unsigned src_data_R = (src_data & 0x00ff0000);
		unsigned src_data_G = (src_data & 0x0000ff00);
		unsigned src_data_B = (src_data & 0x000000ff);

		unsigned dst_data_A = (dst_data && 0xff000000) >> 24;
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
}

void VisualGame::drawTheme() {
	DDS& img = Game::getImg(IMG_THEME);
	drawCell(0, 0, img);
}

namespace GameLib {
	VisualGame* p_visualGame = nullptr;
	const int FPS = 60; // 16.66 ms 每一Frame
	bool var_fps = true;
	bool should_draw_theme = true; // 是否绘制菜单
	GameState game_state;
	
	// 主循环
	void mainLoop() { // 主游戏循环
		static unsigned previous_time[FPS]{}; // 前一次的时间戳
		static int counter = 0; // 游戏循环次数
		static bool initialized = false;
		Framework framework = Framework::instance();

		if (!initialized) {
			p_visualGame = new VisualGame();
			p_visualGame->init(MapSource::FILE, var_fps);
			initialized = true;
			GameLib::cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << GameLib::endl;
		}
		GameLib::cout << "第" << ++counter << "次更新" << endl;
		if (var_fps)
			p_visualGame->update(framework.time() - previous_time[FPS - 1]); // 这里最好睡眠一下
		else
			p_visualGame->update();
		p_visualGame->draw();
		if (p_visualGame->is_finished())
		{
			GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << p_visualGame->steps_ << "." << GameLib::endl;
			game_state = GameState::THEME;
			p_visualGame->reset_game(MapSource::FILE,var_fps);
		}
		if(framework.isKeyTriggered('q')|| framework.isKeyTriggered('Q'))
			framework.requestEnd();

		if (framework.isEndRequested()) {
			delete p_visualGame;
			p_visualGame = nullptr;
			GameLib::cout << "Goodbye!" << GameLib::endl;
			exit(0); // 临时处理，防止按了Q之后再按其他的按键会造成指针访问错误
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
	
	// 菜单循环
	void titleLoop() {
		p_visualGame->drawTheme();
		if(Framework::instance().isKeyTriggered('m')|| Framework::instance().isKeyTriggered('M'))
			game_state = GameState::GAME;
		if (Framework::instance().isKeyTriggered('q') || Framework::instance().isKeyTriggered('Q'))
			Framework::instance().requestEnd();
	}

	// 框架循环
	void Framework::update() {
		switch (game_state) {
		case GameState::THEME:
			titleLoop();
			break;
		case GameState::GAME:
			mainLoop();
			break;

		}      
	}
}
