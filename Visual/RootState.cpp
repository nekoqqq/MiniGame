#include <cassert>
#include "RootState.h"
#include "ThemeState.h"
#include "GamePlayState.h"
#include "GoodEndingState.h"
#include "BadEndingState.h"
#include "GameLib/Framework.h"
#include "GameSound.h"


GameContext& GameContext::instance()
{
    static GameContext instance_;
    return instance_;
}
GameContext::GameContext() : state_(THEME),mode_(P1) {
    current_state_ = new ThemeState();
}
GameContext::~GameContext() { delete current_state_; }
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
    Base* next_state = current_state_->update(this);
    if (next_state != current_state_) {
        DYNAMIC_DEL(current_state_);
        current_state_ = dynamic_cast<SubState*> (next_state);
    }
}

Base* SubState::update(Base* p) {
    GameContext* parent = dynamic_cast<GameContext*> (p);
    ASSERT(parent);
    return update(parent);
}
Base* SubSubState::update(Base* p) {
    GamePlayState* parent = dynamic_cast<GamePlayState*>(p);
    ASSERT(parent);
    return update(parent);
}