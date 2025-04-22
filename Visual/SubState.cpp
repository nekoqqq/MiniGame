#include "SubState.h"
#include "../Console/DDS.h"
#include "GameLib/Framework.h"
#include "HSMGame.h"
#include "MainState.h"
#include "RootState.h"

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
    while (framework.time() - game_world->getLastFrameCost() < 1000 / game_world->FPS)
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

MenuState::MenuState()
{
    menu_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\menu.dds");
}
MenuState::~MenuState()
{
    DYNAMIC_DEL(menu_img);
}
void MenuState::update(MainState* parent) {
    Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
    if (f.isKeyTriggered('1')) { // 重置
        parent->getHSMGame()->reset();
        parent->setState(MainState::PLAY);
    }
    else if (f.isKeyTriggered('2')) { // 选关
        parent->setState(MainState::SELECTION);
    }
    else if (f.isKeyTriggered('3')) { // 回到主题
        parent->setState(MainState::THEME);
    }
    else if (f.isKeyTriggered('4')) { // 继续
        parent->setState(MainState::PLAY);
    }
    menu_img->drawImage();
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
