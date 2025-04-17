#pragma once

#include "../Console/Game.h"
#include "resource.h"

class VisualGame :public Game {
public:
	virtual void update(string&);
	virtual void update();
	void update(int); //  可变FPS
	virtual void draw();
protected:
	void drawFPS();
};