#include "RootState.h" 
#include "MainState.h"
#include "GameLib/Framework.h"
#include "SubState.h"
#include "HSMGame.h"
#include "GoodEndingState.h"
#include "BadEndingState.h"
#include "BomberGame.h"

MainState::MainState(int stage) : stage_(stage) {
    hsm_game_ = new HSMGame(stage_,true);
    bomber_game_ = &BomberGame::instance();
    bomber_game_->setStage(stage);
    p1_loading_state_ = new P1LoadingState();
    play_state_ = nullptr;
    menu_state_ = nullptr;
    succeed_state_ = nullptr;
    failed_state_ = nullptr;
    outcome_state_ = nullptr;
}
MainState::~MainState()
{
    DYNAMIC_DEL(hsm_game_);
    DYNAMIC_DEL(p1_loading_state_);
    DYNAMIC_DEL(play_state_);
    DYNAMIC_DEL(menu_state_);
    DYNAMIC_DEL(succeed_state_);
    DYNAMIC_DEL(failed_state_);
    DYNAMIC_DEL(outcome_state_);
}
MainState::Mode MainState::getMode() const
{
    if (RootState::instance().getMode() == RootState::P1)
        return MainState::P1;
    else if (RootState::instance().getMode() == RootState::P2)
        return MainState::P2;
    else
        return MainState::UNK;
}
void MainState::setState(State state) {
    state_ = state;
}
HSMGame* MainState::getHSMGame() const {
    return hsm_game_;
}
BomberGame* MainState::getBomberGame() const
{
    return bomber_game_;
}
void MainState::setStage(int stage)
{
    stage_ = stage;
}
int MainState::getStage() const {
    return stage_;
}
bool MainState::isFinishAllStage() const
{
    return stage_ == TOTAL_STAGE + 1;
}
void MainState::setTrial(int trial)
{
    trial_ = trial;
}
int MainState::getTrial() const
{
    return trial_;
}
bool MainState::isFailed() const
{
    for (auto& d : bomber_game_->getDynamicObject())
        if (d.getType() == BomberObject::P1_PLAYER)
            return false;
    return true;
}
void MainState::update(RootState* parent) {
    switch (state_) {
    case P1_LOADING: // 全局初始化、过关、失败、双人游戏结束、不动
        ASSERT(
            (p1_loading_state_ && !(menu_state_ || play_state_ || succeed_state_ || failed_state_ || outcome_state_))
            ||
            (succeed_state_ && !(p1_loading_state_ || menu_state_ || play_state_ || failed_state_ || outcome_state_))
            ||
            (failed_state_ && !(p1_loading_state_ || menu_state_ || play_state_ || succeed_state_ || outcome_state_))
            ||
            (outcome_state_ && !(p1_loading_state_ || menu_state_ || play_state_ || succeed_state_ || failed_state_))
            
        );
        if (!p1_loading_state_ && succeed_state_) {
            DYNAMIC_DEL(succeed_state_);
            p1_loading_state_ = new P1LoadingState();
        }
        else if (!p1_loading_state_ && failed_state_) {
            DYNAMIC_DEL(failed_state_);
            p1_loading_state_ = new P1LoadingState();
        }
        else if (!p1_loading_state_ && outcome_state_) {
            DYNAMIC_DEL(outcome_state_);
            p1_loading_state_ = new P1LoadingState();
        }
        p1_loading_state_->update(this);
        break;
    case PLAY: // 读取、菜单、不动
        ASSERT(
            (p1_loading_state_ && !(play_state_ || menu_state_ || succeed_state_ || failed_state_ || outcome_state_))
            ||
            (menu_state_) && !(p1_loading_state_ || play_state_ || succeed_state_ || failed_state_ || outcome_state_)
            ||
            (play_state_ && !(p1_loading_state_ || menu_state_ || succeed_state_ || failed_state_ || outcome_state_))
        );
        if (!play_state_ && p1_loading_state_) {
            DYNAMIC_DEL(p1_loading_state_);
            play_state_ = new PlayState();
        }
        else if (!play_state_ && menu_state_) {
            DYNAMIC_DEL(menu_state_);
            play_state_ = new PlayState();
        }
        play_state_->update(this);
        break;
    case MENU: // 游戏、不动
        ASSERT(
            (play_state_ && !(menu_state_ ||p1_loading_state_  || succeed_state_ || failed_state_ || outcome_state_))
            ||
            (menu_state_ && !(play_state_|| p1_loading_state_  || succeed_state_ || failed_state_ || outcome_state_))
        );
        if (!menu_state_ && play_state_) {
            DYNAMIC_DEL(play_state_);
            menu_state_ = new MenuState();
        }
        menu_state_->update(this);
        break;
    case SUCCEED: // 游戏、不动
        ASSERT(
            (play_state_ && !(menu_state_ || p1_loading_state_ || succeed_state_ || failed_state_ || outcome_state_))
            ||
            (succeed_state_ && !(p1_loading_state_ || menu_state_ || play_state_ || failed_state_ || outcome_state_))
        );
        if (!succeed_state_ && play_state_) {
            DYNAMIC_DEL(play_state_);
            succeed_state_ = new SucceedState();
            setStage(stage_ + 1);
        }
        succeed_state_->update(this);
        break;
    case FAILED: // 游戏、不动
        ASSERT(
            (play_state_ && !(menu_state_ || p1_loading_state_ || succeed_state_ || failed_state_ || outcome_state_))
            ||
            (failed_state_ && !(p1_loading_state_ || menu_state_ || play_state_ || succeed_state_ || outcome_state_))
        );
        if (!failed_state_ && play_state_) {
            DYNAMIC_DEL(play_state_);
            failed_state_ = new FailedState();
        }
        failed_state_->update(this);
        break;
    case OUTCOME: // 游戏、不动
        ASSERT(
            (play_state_ && !(menu_state_ || p1_loading_state_ || succeed_state_ || failed_state_ || outcome_state_))
            ||
            outcome_state_ && !(play_state_|| menu_state_ || p1_loading_state_ || succeed_state_ || failed_state_)
        );
        if (!outcome_state_ && play_state_) {
            DYNAMIC_DEL(play_state_);
            outcome_state_ = new OutcomeState();
        }
        outcome_state_->update(this);
        break;
    // 上层状态
    case THEME: // 菜单、结果、不动(只会向上转发一次，不会实际处理)
        ASSERT(
            (menu_state_ && !(play_state_ || p1_loading_state_ || succeed_state_ || failed_state_ || outcome_state_))
            ||
            (outcome_state_ && !(play_state_ || p1_loading_state_ || succeed_state_ || failed_state_ || menu_state_))
        );
        if (menu_state_) {
            DYNAMIC_DEL(menu_state_);
        }
        else if (outcome_state_) {
            DYNAMIC_DEL(outcome_state_);
        }
        parent->setState(RootState::THEME);
        break;
    case GOOD_ENDING: // 游戏、不动
        ASSERT(
            (succeed_state_ && !(menu_state_ || p1_loading_state_ || play_state_ || failed_state_ || outcome_state_))
        );
        DYNAMIC_DEL(play_state_);
        parent->setState(RootState::GOOD_ENDING);
        break;
    case BAD_ENDING:
        ASSERT(
            (failed_state_ && !(menu_state_ || p1_loading_state_ || play_state_ || succeed_state_ || outcome_state_))
        );
        DYNAMIC_DEL(play_state_);
        parent->setState(RootState::BAD_ENDING);
        break;
    }
}

