#include "RootState.h"
#include "BadEndingState.h"
#include "GameLib/Framework.h"
#include "StringDrawer.h"
#include "../Console/DDS.h"
#include "ThemeState.h"

using GameLib::Framework;

BadEndingState::BadEndingState() {
    failure_img =std::make_unique<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds");
    failure_start_time = Framework::instance().time();
}

BadEndingState::~BadEndingState() = default;

GameState* BadEndingState::update(GameContext* root_context)
{
    GameState* next_state = this;
    if (Framework::instance().time() - failure_start_time > 2000) { // 显示1秒失败logo
        root_context->setState(GameContext::THEME); // 失败则迁移到主题
        next_state = new ThemeState();
    }
    failure_img->drawImage();
    StringDrawer::instance().drawStringAt(0, 0, "SORRY!");
    return next_state;
}