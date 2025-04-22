#include "RootState.h" 
#include "MainState.h"
#include "GameLib/Framework.h"
#include "SubState.h"
#include "HSMGame.h"

MainState::MainState(int stage) :state_(LOADING), stage_(stage) {
    hsm_game_ = new HSMGame(stage_,true);
    loading_state_ = new LoadingState();
    play_state_ = nullptr;
    menu_state_ = nullptr;
    ending_state_ = nullptr;
}
MainState::~MainState()
{
    DYNAMIC_DEL(hsm_game_);
    DYNAMIC_DEL(loading_state_);
    DYNAMIC_DEL(play_state_);
    DYNAMIC_DEL(menu_state_);
    DYNAMIC_DEL(ending_state_);
}
void MainState::setState(State state) {
    state_ = state;
}
HSMGame* MainState::getHSMGame() const {
    return hsm_game_;
}
int MainState::getStage() const {
    return stage_;
}
void MainState::update(RootState* parent) {
    switch (state_) {
    case LOADING:
        if (menu_state_) {
            DYNAMIC_DEL(menu_state_);
            loading_state_ = new LoadingState();
        }
        loading_state_->update(this);
        break;
    case PLAY:
        if (loading_state_ || menu_state_) {
            // 下面的只有一个会执行
            DYNAMIC_DEL(loading_state_);
            DYNAMIC_DEL(menu_state_);
            play_state_ = new PlayState();
        }
        play_state_->update(this);
        break;
    case MENU:
        if (play_state_) {
            DYNAMIC_DEL(play_state_);
            menu_state_ = new MenuState();
        }
        menu_state_->update(this);
        break;
    case ENDING:
        if (play_state_) {
            DYNAMIC_DEL(play_state_);
            ending_state_ = new EndingState();
        }
        ending_state_->update(this);
        break;
    case THEME:
        parent->setState(RootState::THEME);
        break;
    case SELECTION:
        parent->setState(RootState::SELECTION);
        break;
    }
}

