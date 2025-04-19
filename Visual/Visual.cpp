// Visual.cpp : 定义应用程序的入口点。
//

#include "Visual.h"
#include "GameLib/Framework.h"
using namespace std;
using namespace GameLib;

VisualGame::VisualGame(int stage,bool var_fps):Game(MapSource::FILE,var_fps,stage),elapsed_time(0),move_count(0),var_move_count(0){}
VisualGame::~VisualGame(){Game::~Game();}
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
	int window_width = Framework::instance().width();
	unsigned* p_img = dds.get_image_data();
	int img_width = dds.get_image_width();
	int img_height = dds.get_image_height();
	// TODO 这里的混合有问题，最终总是画面偏黄, 已经修复，是因为读取图片的API有问题
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
	}

	if (var_move_count > 0) {
		var_move_count = min(var_move_count + t, MAX_VAR_MOVE_COUNT); // 后续需要用MAX_VAR_MOVE_COUNT减去相应的值，因此需要这里做一下处理
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
	}

	if (move_count > 0) {
		move_count++;
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
void VisualGame::updateState(pair<int,int> &delta)
{
	if (delta.first == 0 && delta.second == 0)
		return;
	if(!move_count||!var_move_count)
		setMove();
}


