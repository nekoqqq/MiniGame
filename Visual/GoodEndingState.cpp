#include "GoodEndingState.h"
#include "RootState.h"
#include "../Console/DDS.h"
#include "GameLib/Framework.h"
#include "StringDrawer.h"
using GameLib::Framework;


GoodEndingState::GoodEndingState()
{
	good_ending_img = std::make_unique<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds");
	good_ending_start_time = Framework::instance().time();
}
void GoodEndingState::update(RootState* parent)
{
	if (Framework::instance().time() - good_ending_start_time > 1000) {
		parent->setState(RootState::THEME);
	}
	good_ending_img->drawImage();
	StringDrawer::instance().drawStringAt(0, 0, "CONGRATULATIONS!");
	StringDrawer::instance().drawStringAt(1, 0, "YOU FINISH ALL TASKS!");
}
GoodEndingState::~GoodEndingState() = default;