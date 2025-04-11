// Visual.cpp : 定义应用程序的入口点。
//

#include "GameLib/Framework.h"
#include<iostream>
using namespace std;
namespace GameLib {
	void Framework::update() {
		unsigned* vram = videoMemory();
		int w = width();
		int h = height();
		int x = rand() % w;
		int y = rand() % h;
		int color = rand() % (1 << 23);
		vram[y * w + x] = color;
	}
}

int main() {
	GameLib::Framework framework = GameLib::Framework::instance();
	while (true) {
		framework.update();
	}
}

