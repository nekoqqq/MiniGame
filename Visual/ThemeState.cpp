#include "ThemeState.h"
#include "RootState.h"
#include "../Console/DDS.h"
#include "GameLib/Framework.h"
#include "StringDrawer.h"
#include <sstream>
using GameLib::Framework;

ThemeState::ThemeState():selection(1) {
    theme_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\main_theme.dds");
}
ThemeState::~ThemeState() {
    DYNAMIC_DEL(theme_img);
}
void ThemeState::update(RootState* parent) {
    Framework f = Framework::instance();

    if (f.isKeyTriggered('w') || f.isKeyTriggered('W') || f.isKeyTriggered('s') || f.isKeyTriggered('S')) {
        selection = selection % 2 + 1;
    }
    else if (f.isKeyTriggered(' ')) // 替换成空格键开始
    {
        if (selection == 1)
            parent->setMode(RootState::P1);
        else if (selection == 2)
            parent->setMode(RootState::P2);
        parent->setState(RootState::MAIN);
    }
    theme_img->drawImage();
    const char* option[] = { "<Welcome!>","1P GAME","2P GAME" };
    std::stringstream oss;
    for (int i = 0; i < 3; i++) {
        if (i == selection) {
            oss << ":>" << option[i] << std::endl;
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str() , 0xff0000);
        }
        else {
            oss << "  " << option[i] << std::endl;
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str(), 0x00ff00);
        }
        oss.str("");
    }
}
