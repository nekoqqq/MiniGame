#include "ThemeState.h"
#include "RootState.h"
#include "../Console/DDS.h"
#include "GameLib/Framework.h"

ThemeState::ThemeState() {
    theme_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\main_theme.dds");
}
ThemeState::~ThemeState() {
    DYNAMIC_DEL(theme_img);
}
void ThemeState::update(RootState* parent) {
    if (GameLib::Framework::instance().isKeyTriggered(' ')) // 替换成空格键开始
    {
        parent->setState(RootState::SELECTION);
    }
    theme_img->drawImage();
}
