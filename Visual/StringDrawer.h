#pragma once
#include<string>
#include "../Console/DDS.h"

class StringDrawer {
public:
	static StringDrawer* instance() {
		if (!instance_)
			instance_ = new StringDrawer("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\font.dds");
		return instance_;
	}




	void drawString(const char * str) {
		// 字符的宽和高
		const int char_width = 16;
		const int char_heigth = 16;

	}

private:
	static StringDrawer* instance_;
	DDS* font_img;
	StringDrawer(const char* file_name)
	{
		font_img = new DDS(file_name);
	}
	~StringDrawer()
	{
		delete instance_;
		instance_ = nullptr;
	}
	StringDrawer(const StringDrawer&);
};