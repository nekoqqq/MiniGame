﻿// Visual.cpp : 定义应用程序的入口点。
//

#include "../Console/Game.h"
#include "../Console/GameObject.h"
#include "../Console/DDS.h"
#include "VisualGame.h"

#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib::Input;
using namespace std;
using namespace GameLib;

VisualGame::VisualGame(int stage,bool var_fps):Game(MapSource::FILE,var_fps,stage),elapsed_time(0),move_count(0),var_move_count(0){}
VisualGame::~VisualGame(){Game::~Game(); }
void VisualGame::draw() { // 同时向控制台和图形界面输出，控制台是用来debug的
	// 先绘制背景
	for(int i =0;i<getHeight();i++)
		for (int j = 0; j < getWidth(); j++) {
			GameObject& go = getGameObject(i, j);
			// 修复一个bug，原本是希望除了墙壁以外都涂上空白，但是因为Target、Box这些东西都是透明的，会显示背景，所以最好也将这些位置绘制好
			// 但是如果先处理好图片，让Target、Box这些不透明，就可以省略这些东西混合的计算过程
			if (go != GameObject::BOUNDARY ) {
				DDS& img = VisualGame::getImg(IMG_TYPE::IMG_BLANK); 
				drawCell(i * 48, j * 48, img);
			}
			else {
				DDS& img = VisualGame::getImg(IMG_TYPE::IMG_BOUNDARY); // 这里的一个trick是因为在类的实现里面，所以getImg是基类的方法，不需要实例化一个类才可以使用（继承的好处！）
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
			if (!(go == GameObject::BLANK || go == GameObject::BOUNDARY)) // 玩家或者箱子移动
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
	dds.drawCell(src_x, src_y);
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
bool VisualGame::varPreHandle(int t)
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
		return false;
	}
	return true;
}
Game::DIRECTION VisualGame::handleInput() {
	DIRECTION direction = UNKNOW;
	GameLib::Framework framework = GameLib::Framework::instance();
	Keyboard k = Manager::instance().keyboard();
	// 当前按键输入
	bool cur_key_on_w = (k.isTriggered('w') || k.isTriggered('W'));
	bool cur_key_on_a = (k.isTriggered('a') || k.isTriggered('A'));
	bool cur_key_on_s = (k.isTriggered('s') || k.isTriggered('S'));
	bool cur_key_on_d = (k.isTriggered('d') || k.isTriggered('D'));

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

bool VisualGame::fixPreHandle()
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
		return false;
	}
	return true;
}
bool VisualGame::preHandle()
{
	if (isGameVar()) {
		return varPreHandle(elapsed_time);
	}
	else
		return fixPreHandle();
}
void VisualGame::updateState(pair<int,int> &delta)
{
	if (delta.first == 0 && delta.second == 0)
		return;
	if(!move_count||!var_move_count)
		setMove();
}