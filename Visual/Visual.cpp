// Visual.cpp : 定义应用程序的入口点。
//

#include "Visual.h"
#include "GameLib/Framework.h"
using namespace std;
using namespace GameLib;

VisualGame::VisualGame(int stage,bool var_fps):Game(MapSource::FILE,var_fps,stage),elapsed_time(0),move_count(0),var_move_count(0){}
void VisualGame::draw() { // 同时向控制台和图形界面输出，控制台是用来debug的
	// 先绘制背景
	for(int i =0;i<getHeight();i++)
		for (int j = 0; j < getWidth(); j++) {
			GameObject& go = getGameObject(i, j);
			if (go == GameObject::BLANK || go == GameObject::BOUNDARY || go == GameObject::TARGET) {
				IMG_TYPE img_type = go.getImgType();
				DDS& img = VisualGame::getImg(img_type); // 这里的一个trick是因为在类的实现里面，所以getImg是基类的方法，不需要实例化一个类才可以使用（继承的好处！）
				drawCell(i * 48, j * 48, img);
			}
		}


	// 再绘制前景，用于修复从下往上，因为先在上面的格子绘制了人物，后在下面的给子绘制了背景，导致人物的下半身没有了的问题
	for (int i = 0; i < getHeight(); i++)
		for (int j = 0; j < getWidth(); j++)
		{
			GameObject& go = getGameObject(i,j);
			IMG_TYPE img_type = go.getImgType();
			DDS& img = VisualGame::getImg(img_type);
			if (!(go == GameObject::BLANK || go == GameObject::BOUNDARY || go == GameObject::TARGET)) // 玩家或者箱子移动
			{
				if (isGameVar()) { // 可变和固定的计算方式不一样，固定是每Frame移动一个固定的像素，而可变的是根据每个Frame的实际消耗时间，移动相应的距离
					int move_dx = go.get_move().first, move_dy = go.get_move().second;
					drawCell(i * 48 - 48 * (MAX_VAR_MOVE_COUNT - var_move_count) * move_dx / MAX_VAR_MOVE_COUNT, j * 48 - 48 * (MAX_VAR_MOVE_COUNT - var_move_count) * move_dy / MAX_VAR_MOVE_COUNT, img);
				}
				else {
					int move_dx = go.get_move().first, move_dy = go.get_move().second;
					drawCell(i * 48 - (48 - move_count) * move_dx, j * 48 - (48 - move_count) * move_dy, img);
				}
					
			}
		}
	
	// STD debug out
	for (int i = 0; i < getHeight(); i++, GameLib::cout << endl)
		for (int j = 0; j < getWidth(); j++) {
			GameLib::cout << static_cast<char>(getGameObject(i,j));
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
void VisualGame::drawTheme(DDS &theme_img) {
	drawCell(0, 0, theme_img);
}
void VisualGame::set_elapsed_time(int elapsed_time)
{
	this->elapsed_time = elapsed_time;
}
void VisualGame::setMove()
{
	this->move_count = 1;
	this->var_move_count = 1;
}
void VisualGame::varPreHandle(int t)
{
	if (var_move_count >= MAX_VAR_MOVE_COUNT) {
		var_move_count = 0;
		for (int i = 0; i < getHeight(); i++)
			for (int j = 0; j < getWidth(); j++) {
				getGameObject(i, j).set_move(0, 0);
			}
		setShouldSkip(false);
	}

	if (var_move_count > 0) {
		var_move_count = min(var_move_count + t, MAX_VAR_MOVE_COUNT); // 后续需要用MAX_VAR_MOVE_COUNT减去相应的值，因此需要这里做一下处理
		setShouldSkip(true);
	}
}
Game::DIRECTION VisualGame::handleInput() {
	DIRECTION direction = UNKNOW;
	GameLib::Framework framework = GameLib::Framework::instance();
	// 当前按键输入
	bool cur_key_on_w = (framework.isKeyTriggered('w') || framework.isKeyTriggered('W'));
	bool cur_key_on_a = (framework.isKeyTriggered('a') || framework.isKeyTriggered('A'));
	bool cur_key_on_s = (framework.isKeyTriggered('s') || framework.isKeyTriggered('S'));
	bool cur_key_on_d = (framework.isKeyTriggered('d') || framework.isKeyTriggered('D'));

	if (cur_key_on_w)
		direction = UP;
	else if (cur_key_on_a)
		direction = LEFT;
	else if (cur_key_on_s)
		direction = DOWN;
	else if (cur_key_on_d)
		direction = RIGHT;
	GameLib::cout << "direction: " << direction << GameLib::endl;
	return direction;
}

void VisualGame::fixPreHandle()
{
	if (move_count == 48) {
		move_count = 0;
		for (int i = 0; i < getHeight(); i++)
			for (int j = 0; j < getWidth(); j++) {
				GameObject& gameObject = getGameObject(i, j);
				gameObject.set_move(0, 0);
			}
		setShouldSkip(false);
	}

	if (move_count > 0) {
		move_count++;
		setShouldSkip(true);
	}
}
void VisualGame::preHandle()
{
	if (isGameVar()) {
		varPreHandle(elapsed_time);
	}
	else
		fixPreHandle();
}
void VisualGame::extraStateHandle()
{
	if(!move_count||!var_move_count)
		setMove();
}


namespace GameLib {
	VisualGame* p_visualGame = nullptr;
	const int FPS = 60; // 16.66 ms 每一Frame
	bool var_fps = true;
	GameState game_state;
	DDS* theme_img = nullptr;
	DDS* selection_img = nullptr;
	DDS* loading_img = nullptr;
	int stage = 0;
	
	// 主循环
	void mainLoop() { // 主游戏循环
		static unsigned previous_time[FPS]{}; // 前一次的时间戳
		static int counter = 0; // 游戏循环次数
		Framework framework = Framework::instance();

		GameLib::cout << "第" << ++counter << "次更新" << endl;
		if (!p_visualGame) {
			p_visualGame = new VisualGame(stage,var_fps);
			previous_time[FPS - 1] = framework.time();
			GameLib::cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << GameLib::endl;
		}

		p_visualGame->set_elapsed_time(framework.time() - previous_time[FPS - 1]); // 这里最好睡眠一下
		p_visualGame->update();
		p_visualGame->draw();
		if (p_visualGame->is_finished())
		{
			GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << p_visualGame->steps_ << "." << GameLib::endl;
			game_state = GameState::THEME;
			SAFE_DELETE(p_visualGame);
		}
		if(framework.isKeyTriggered('q')|| framework.isKeyTriggered('Q'))
			framework.requestEnd();

		if (framework.isEndRequested()) {
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
	
	// 主题循环
	void themeLoop() {
			if (!theme_img)
				theme_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\main_theme.dds");
			p_visualGame->drawTheme(*theme_img);
			if (Framework::instance().isKeyTriggered(32)) // 替换成空格键开始
			{
				game_state = GameState::SELECTION;
				SAFE_DELETE(theme_img); // 这里如果只用了delete theme_img，那么只是所指向的空间被释放了，但是theme_img这个指针不为空，因此上面的判断仍然可以通过，这也是为什么还需要让theme_img为nullptr
			}
			
		if (Framework::instance().isKeyTriggered('q') || Framework::instance().isKeyTriggered('Q'))
			Framework::instance().requestEnd();
	}

	void selectionLoop() {
		if (!selection_img)
			selection_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\selection.dds");
		p_visualGame->drawTheme(*selection_img);
		Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
		for (char c : "123456789") {
			if (f.isKeyTriggered(c)) {
				stage = c - '0';
				game_state = GameState::LOADING;
				SAFE_DELETE(selection_img);
			}
		}
	}

	void loadingLoop() {
		//static DDS* loading_img=nullptr;
		if (!loading_img)
			loading_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\loading.dds");
		p_visualGame->drawTheme(*loading_img);
		Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
		f.sleep(500);
		game_state = GameState::GAME;
		SAFE_DELETE(loading_img);
	}
	// 框架循环
	void Framework::update() {
		switch (game_state) {
		case GameState::THEME:
			themeLoop();
			break;
		case GameState::SELECTION:
			selectionLoop();
			break;
		case GameState::GAME:
			mainLoop();
			break;
		case GameState::LOADING:
			loadingLoop();
			break;
		}      
	}
}
