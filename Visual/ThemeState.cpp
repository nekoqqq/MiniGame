#include "ThemeState.h"
#include "GamePlayState.h"
#include "RootState.h"
#include "../Console/DDS.h"
#include "GameLib/Framework.h"
#include "StringDrawer.h"
#include <sstream>
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Sound/Wave.h"
#include "GameLib/Sound/Manager.h"
#include "GameSound.h"
using namespace GameLib::Input;
using namespace GameLib::Sound;
using GameLib::Framework;

ThemeState::ThemeState():selection(1) {
    theme_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\main_theme.dds");
    GameSound::instance().playBGM(GameSound::THEME);
}
ThemeState::~ThemeState() {
    DYNAMIC_DEL(theme_img);
    GameSound::instance().stopBGM();
}
Base* ThemeState::update(GameContext* parent) {
    Base* next_state = this;
    Keyboard k = GameLib::Input::Manager::instance().keyboard();
    if (k.isTriggered('w') || k.isTriggered('W') || k.isTriggered('s') || k.isTriggered('S')) {
        selection = selection % 2 + 1;
    }
    else if (k.isTriggered(' ')) // 替换成空格键开始
    {
        if (selection == 1)
            parent->setMode(GameContext::P1);
        else if (selection == 2)
            parent->setMode(GameContext::P2);
        parent->setState(GameContext::MAIN);
        next_state = new GamePlayState(1);
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
    return next_state;
}
