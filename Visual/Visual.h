#pragma once

#include "../Console/Game.h"
#include "resource.h"

class VisualGame :public Game {
public:
	virtual void update(string&);
	virtual void update();
	void update(int); //  可变FPS
	virtual void draw();
	void drawTheme();
	bool should_draw_theme = false;
protected:
	void drawFPS();
	void drawCell(int,int,DDS&);
	
};