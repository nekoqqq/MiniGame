#include "StringDrawer.h"

// 单例模式的实现
StringDrawer* StringDrawer::instance_ = nullptr;
StringDrawer* StringDrawer::instance() {
	if (!instance_)
		instance_ = new StringDrawer("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\font.dds", 0x00ffff);
	return instance_;
}
void StringDrawer::drawString(int screen_x, int screen_y, const char* str) { // 在屏幕上的固定位置绘制字符串
	// 字符的宽和高
	for (int i = 0; str[i] != '\0'; i++) {
		int index = (str[i] - ' ');
		if (index < 0 || index >= 95)
			index = 95;
		int src_x = (index / char_element_len) * char_width;
		int src_y = (index % char_element_len) * char_heigth;
		font_img->render(src_x, src_y, char_width, char_heigth, screen_x, screen_y, font_color);
		screen_y += char_width;
		if (screen_y == GameLib::Framework::instance().width()) { // 换行处理
			screen_x += char_heigth;
			screen_y = 0;
		}
	}
}
void StringDrawer::drawStringAt(int i, int j, const char* str) { // 在屏幕的第i行j列绘制文字，不考虑字符大小
	int window_width = GameLib::Framework::instance().width();
	drawString(i * char_heigth, j * char_width, str);
}
StringDrawer::StringDrawer(const char* file_name, unsigned font_color) :font_img(new DDS(file_name)), font_color(font_color) {}
StringDrawer::~StringDrawer()
{
	delete instance_;
	instance_ = nullptr;
}
