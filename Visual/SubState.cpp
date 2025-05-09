#include "../Console/DDS.h"
#include "GameLib/Framework.h"
#include "HSMGame.h"
#include "GamePlayState.h"
#include "RootState.h"
#include "StringDrawer.h"
#include <sstream>
#include "BomberGame.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "SubState.h"
#include "GoodEndingState.h"
#include "BadEndingState.h"
#include "ThemeState.h"
#include "GameSound.h"
using namespace GameLib::Input;
using GameLib::Framework;

P1LoadingState::P1LoadingState()
{
    loading_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\loading.dds");
    loading_start_time = Framework::instance().time();
}
P1LoadingState::~P1LoadingState()
{
    DYNAMIC_DEL(loading_img);
}
Base* P1LoadingState::update(GamePlayState* parent) {
    Base* next_state = this;
    Framework f = Framework::instance();
    if (f.time() - loading_start_time > 2000) {
        BomberGame* game_world = parent->getBomberGame();
        game_world->loadGame(parent->getStage());
        parent->setState(GamePlayState::PLAY);
        next_state = new PlayState();
    }
    else if (f.time() - loading_start_time > 1000) {
        StringDrawer::instance().drawStringAt(1, 0, "READY...", 0xff0000);
    }
    else {
        StringDrawer::instance().drawStringAt(1, 0, "GO!", 0xff0000);
    }
    loading_img->drawImage(); 
    StringDrawer::instance().drawStringAt(0, 0, "<Loading>",0xff0000);
    return next_state;
}

PlayState::PlayState()
{
    GameSound::instance().playBGM(GameSound::GAME);
}
PlayState::~PlayState()
{
    GameSound::instance().stopBGM();
}
// 前处理部分
void PlayState::pre(GamePlayState* parent) {
    HSMGame* game_world = parent->getHSMGame();
    game_world->set_elapsed_time(Framework::instance().time() - game_world->getLastFrameCost());
}
// 后处理部分
void PlayState::post(GamePlayState* parent) {
    // 补FPS
    Framework framework = Framework::instance();
    HSMGame* game_world = parent->getHSMGame();
    game_world->updateStat(framework.time()); // 这个应该是正确的实现，修复了时间统计错误的问题
    while (framework.time() - game_world->getLastFrameCost() < 100 / game_world->FPS)
        framework.sleep(1);
}
Base* PlayState::update(GamePlayState* parent) {
    Base* next_state = this;
    pre(parent);
    Framework framework = Framework::instance();
    Keyboard k = Manager::instance().keyboard();
    BomberGame* game_world = parent->getBomberGame();
    if (parent->getMode() == GamePlayState::P1) {
        if (k.isTriggered('z') || game_world->is_finished()) { // 临时加入过关的处理
            GameLib::cout << "YOU WIN! Total steps(exculude invalid steps): " << game_world->steps_ << "." << GameLib::endl;
            parent->setState(GamePlayState::SUCCEED);
            parent->setStage(parent->getStage() + 1);
            next_state = new SucceedState();
        }
        else if (k.isTriggered('f') || parent->isFailed()) { // 临时加入失败的处理
            parent->setTrial(parent->getTrial() + 1);
            parent->setState(GamePlayState::FAILED);
            next_state = new FailedState();
        }
    }
    else if (parent->getMode() == GamePlayState::P2) {
        if (k.isTriggered('b')) { // 临时加入两个人的处理，任意一个失败
            parent->setState(GamePlayState::OUTCOME);
            next_state = new OutcomeState();
        }
    }
    if (k.isTriggered('m') || k.isTriggered('M')) {
        parent->setState(GamePlayState::MENU);
        next_state = new MenuState();
    }

    game_world->update();
    game_world->draw();
    post(parent);
    return next_state;
}

MenuState::MenuState():cur_selection(1)
{
    menu_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds");
}
MenuState::~MenuState()
{
    DYNAMIC_DEL(menu_img);
}
Base* MenuState::update(GamePlayState* parent) {
    Base* next_state = this;
    // 这里修改了展示的顺序，直觉上来说应该先画图再响应键盘的输入，不然出现输入很快，直接把画面给省略了
    Framework f = Framework::instance(); // 为什么Framework &f = Framework::instance() 会失败？
    Keyboard k = Manager::instance().keyboard();
    if (k.isTriggered('w') || k.isTriggered('s')) {
        cur_selection = cur_selection%2+1;
    }
    else if (k.isTriggered(' ')) {
        switch (cur_selection)
        {
        case 1: // 继续
            parent->setState(GamePlayState::PLAY);
            next_state = new PlayState();
            break;
        case 2: // 回到主题
            parent->setState(GamePlayState::THEME);
            next_state = new ThemeState();
            break;
        }
    }
    menu_img->drawImage();
    std::ostringstream oss;
    oss << "<MENU>" << std::endl;
    oss.str("");
    for (int i = 0; i < 2; i++) {
        if (i+1 == cur_selection) {
            oss << ":>" << menu_str[i] << std::endl;
            StringDrawer::instance().drawStringAt(i+1, 0, oss.str().c_str(), 0xff0000);
        }
        else { // 包含0和其他的输出
            oss << "  "<<menu_str[i] << std::endl;
            StringDrawer::instance().drawStringAt(i+1, 0, oss.str().c_str(), 0x00ff00);
        }
        oss.str("");
    }
    return next_state;
}

OutcomeState::OutcomeState():outcome_img(std::make_unique<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds")){
    selection = 1;
}
Base* OutcomeState::update(GamePlayState*parent) {
    Base* next_state = this;
    Keyboard k = Manager::instance().keyboard();
    if (k.isTriggered('w') || k.isTriggered('W') || k.isTriggered('s') || k.isTriggered('S')) {
        selection = selection %2 +1;
    }
    else if (k.isTriggered(' ')) {
        if (selection == 1) {
            parent->setState(GamePlayState::P1_LOADING);
            next_state = new P1LoadingState();
        }
        else if (selection == 2) {
            parent->setState(GamePlayState::THEME);
        }
        else {
            exit(0);
        }
    }
    outcome_img->drawImage();
    const char* option[2] = {"CONTINUE","BACK TO THEME"};
    StringDrawer::instance().drawStringAt(0, 0, "1P win!");
    std::ostringstream oss;
    for (int i = 0; i < 2; i++) {
        if (i == selection-1) {
            oss << ":>" << option[i]<<std::endl;
            StringDrawer::instance().drawStringAt(i+1, 0, oss.str().c_str(),0xff0000);
        }
        else {
            oss << "  " << option[i]<<std::endl;
            StringDrawer::instance().drawStringAt(i+1, 0, oss.str().c_str(),0x00ff00);
        } 
        oss.str("");
    }
    return next_state;
}

SucceedState::SucceedState():succeed_img(std::make_unique<DDS>("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds"))
{
    succeed_start_time = Framework::instance().time();
}
Base* SucceedState::update(GamePlayState* parent)
{
    Base* next_state = this;
    if (Framework::instance().time() - succeed_start_time > 1000) { // 等待1s
        if (parent->isFinishAllStage()) {
            parent->setState(GamePlayState::GOOD_ENDING);
            next_state = new GoodEndingState();
        }
        else {
            parent->setState(GamePlayState::P1_LOADING);
            next_state = new P1LoadingState();
        }
    }
    succeed_img->drawImage();
    StringDrawer::instance().drawStringAt(0, 0, "TASK CLEARED!");
    return next_state;
}

FailedState::FailedState()
{
    failed_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\background.dds");
    failed_start_time = Framework::instance().time();
}
FailedState::~FailedState()
{
    DYNAMIC_DEL(failed_img);
}
Base* FailedState::update(GamePlayState* parent) {
    Base* next_state = this;
    Framework f = Framework::instance();
    if (f.time() - failed_start_time > 1000) { // 显示1秒钟
        if (parent->isFailed()) {
            parent->setState(GamePlayState::BAD_ENDING);
            next_state = new BadEndingState();
        }
        else {
            parent->setState(GamePlayState::P1_LOADING);
            next_state = new P1LoadingState();
        }
    }
    failed_img->drawImage();
    StringDrawer::instance().drawStringAt(0, 0, "TASK FAILED!");
    return next_state;
}