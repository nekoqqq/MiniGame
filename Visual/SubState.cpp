#include "SubState.h"
#include "../Console/DDS.h"
#include "GameLib/Framework.h"
#include "HSMGame.h"
#include "MainState.h"
#include "RootState.h"
#include "StringDrawer.h"
#include <sstream>

using GameLib::Framework;

LoadingState::LoadingState()
{
    loading_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\loading.dds");
    loading_start_time = Framework::instance().time();
}
LoadingState::~LoadingState()
{
    DYNAMIC_DEL(loading_img);
}
void LoadingState::update(MainState* parent) {
    if (Framework::instance().time() - loading_start_time > 1000) {
        HSMGame* game_world = parent->getHSMGame();
        game_world->loadGame(parent->getStage());
        parent->setState(MainState::PLAY);
    }
    loading_img->drawImage(); 
    StringDrawer::instance().drawStringAt(4, 4, "Loading...",0xff0000);
}

// 前处理部分
void PlayState::pre(MainState* parent) {
    HSMGame* game_world = parent->getHSMGame();
    game_world->set_elapsed_time(Framework::instance().time() - game_world->getLastFrameCost());
}
// 后处理部分
void PlayState::post(MainState* parent) {
    // 补FPS
    Framework framework = Framework::instance();
    HSMGame* game_world = parent->getHSMGame();
    game_world->updateStat(framework.time()); // 这个应该是正确的实现，修复了时间统计错误的问题
    while (framework.time() - game_world->getLastFrameCost() < 100 / game_world->FPS)
        framework.sleep(1);
}
void PlayState::update(MainState* parent) {
    pre(parent);
    Framework framework = Framework::instance();
    HSMGame* game_world = parent->getHSMGame();
    if (framework.isKeyTriggered('m') || framework.isKeyTriggered('M')) {
        parent->setState(MainState::MENU);
    }
    else {
        game_world->update();
        if (game_world->is_finished()) {
            GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << game_world->steps_ << "." << GameLib::endl;
            parent->setState(MainState::ENDING);
        }
    }
    game_world->draw();
    post(parent);
}

MenuState::MenuState():cur_selection(1)
{
    menu_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\menu.dds");
}
MenuState::~MenuState()
{
    DYNAMIC_DEL(menu_img);
}
void MenuState::update(MainState* parent) {
    // 这里修改了展示的顺序，直觉上来说应该先画图再响应键盘的输入，不然出现输入很快，直接把画面给省略了
    Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
    if (f.isKeyTriggered('w')) {
        cur_selection = (cur_selection + 2) % (menu_size-1) + 1;
    }
    else if (f.isKeyTriggered('s')) {
        cur_selection = cur_selection % (menu_size-1) + 1;
    }
    else if (f.isKeyTriggered(' ')) {
        switch (cur_selection)
        {
        case 1: // 重置
            parent->getHSMGame()->reset();
            parent->setState(MainState::PLAY);
            break;
        case 2: // 选关
            parent->setState(MainState::SELECTION);
            break;
        case 3: // 回到主题
            parent->setState(MainState::THEME);
            break;
        case 4: // 继续
            parent->setState(MainState::PLAY);
            break;
        default:
            exit(0);
        }
    }
    menu_img->drawImage();
    std::ostringstream oss;
    for (int i = 0; i < menu_size; i++) {
        if (i == cur_selection) {
            oss << ":>" << menu_str[i] << std::endl;
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str(), 0xff0000);
        }
        else { // 包含0和其他的输出
            oss << "  "<<menu_str[i] << std::endl;
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str(), 0x00ff00);
        }
        oss.str("");
    }
}

EndingState::EndingState()
{
    ending_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\ending.dds");
    ending_start_time = Framework::instance().time();
}
EndingState::~EndingState()
{
    DYNAMIC_DEL(ending_img);
}
void EndingState::update(MainState* parent) {
    if (Framework::instance().time() - ending_start_time > 2000) { // 显示2秒钟
        parent->setState(MainState::THEME);
    }
    ending_img->drawImage();
}
