#pragma once
#include "../Console/DDS.h"
#include "GameLib/Framework.h"

class StringDrawer {
public:
	// 单例模式的实现
	static StringDrawer* instance();
	void drawString(int screen_x,int screen_y,const char* str);
	void drawStringAt(int i, int j, const char* str);
private:
	static StringDrawer* instance_;
	DDS* font_img;
	unsigned font_color;
	StringDrawer(const char* file_name,unsigned font_color);
	~StringDrawer();
	StringDrawer(const StringDrawer&);

	const int char_width = 16;
	const int char_heigth = 16;
	const int char_element_len = 16; // 设定每个字符在图片中的长宽都一样
};
// inline的函数必须写在头文件里面，而static定义的内容需要写在cpp文件里面
// 因为inline相当于宏替换，只在本文件里面生效？