#pragma once

#include "../Console/Game.h"
#include "resource.h"

class VisualGame :public Game {
public:
	virtual void update(string&);
	virtual void update();
	virtual void draw();
	void drawFPS();
};