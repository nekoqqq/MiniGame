#include "RootState.h"
#include "BadEndingState.h"
#include "GameLib/Framework.h"
#include "StringDrawer.h"
#include "../Console/DDS.h"

using GameLib::Framework;

BadEndingState::BadEndingState() {
    failure_img =std::make_unique<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds");
    failure_start_time = Framework::instance().time();
}

BadEndingState::~BadEndingState() = default;

void BadEndingState::update(RootState* parent)
{
    if (Framework::instance().time() - failure_start_time > 2000) { // 显示1秒失败logo
        parent->setState(RootState::THEME); // 失败则迁移到主题
    }
    failure_img->drawImage();
    StringDrawer::instance().drawStringAt(0, 0, "SORRY!");
}