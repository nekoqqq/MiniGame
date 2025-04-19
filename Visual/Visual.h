#pragma once

#include "../Console/Game.h"
#include "resource.h"

class VisualGame :public Game {
public:
	explicit VisualGame(int stage, bool var_fps);
	virtual ~VisualGame()override;
	virtual void draw() override;
	void set_elapsed_time(int);
	void drawTheme(DDS&);
	static void drawCell(int, int, DDS&);
	void drawFPS();

private:
	int move_count;
	int var_move_count; // 可变FPS计数器
	const int MAX_VAR_MOVE_COUNT = 50; // 50 ms 根据时间移动相应的像素,50ms移动32个像素, 则速度为640px/s

	int elapsed_time; // 帧之间的时间间隔

	// update里面需要更新的逻辑
	virtual void preHandle() override; // 这里最好使用override关键词，来保证一定重写的是基类的虚函数，而不是重写基类的普通函数
	virtual DIRECTION handleInput()override;
	virtual void updateState(pair<int,int> &) override;

	void setMove();
	void varPreHandle(int);
	void fixPreHandle();
};