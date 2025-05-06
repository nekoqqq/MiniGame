#include <cassert>
#include "RootState.h"
#include "ThemeState.h"
#include "GamePlayState.h"
#include "GoodEndingState.h"
#include "BadEndingState.h"
#include "GameLib/Framework.h"


GameContext& GameContext::instance()
{
    static GameContext instance_;
    return instance_;
}


GameContext::GameContext() : state_(THEME),mode_(P1) {
    current_state_ = new ThemeState();
}
void GameContext::setState(State state) {
    state_ = state;
}

void GameContext::setMode(Mode mode)
{
    mode_ = mode;
}

GameContext::Mode GameContext::getMode()const {
    return mode_;
}

void GameContext::update() {
    GameState* next_state = current_state_->update(this);
    if (next_state != current_state_) {
        DYNAMIC_DEL(current_state_);
        current_state_ = next_state;
    }
    ASSERT(next_state != nullptr);
}

