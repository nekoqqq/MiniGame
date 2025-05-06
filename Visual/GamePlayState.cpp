#include "RootState.h" 
#include "GamePlayState.h"
#include "GameLib/Framework.h"
#include "SubState.h"
#include "HSMGame.h"
#include "GoodEndingState.h"
#include "BadEndingState.h"
#include "BomberGame.h"
#include "ThemeState.h"

GamePlayState::GamePlayState(int stage) : stage_(stage) {
    hsm_game_ = new HSMGame(stage_,true);
    bomber_game_ = &BomberGame::instance();
    bomber_game_->setStage(stage);
    derived_ = new P1LoadingState();
    state_ = P1_LOADING;
}
GamePlayState::~GamePlayState()
{
    DYNAMIC_DEL(derived_);
}
GamePlayState::Mode GamePlayState::getMode() const
{
    if (GameContext::instance().getMode() == GameContext::P1)
        return GamePlayState::P1;
    else if (GameContext::instance().getMode() == GameContext::P2)
        return GamePlayState::P2;
    else
        return GamePlayState::UNK;
}
void GamePlayState::setState(Phase state) {
    state_ = state;
}
HSMGame* GamePlayState::getHSMGame() const {
    return hsm_game_;
}
BomberGame* GamePlayState::getBomberGame() const
{
    return bomber_game_;
}
void GamePlayState::setStage(int stage)
{
    stage_ = stage;
}
int GamePlayState::getStage() const {
    return stage_;
}
bool GamePlayState::isFinishAllStage() const
{
    return stage_ == TOTAL_STAGE + 1;
}
void GamePlayState::setTrial(int trial)
{
    trial_ = trial;
}
int GamePlayState::getTrial() const
{
    return trial_;
}
bool GamePlayState::isFailed() const
{
    for (auto& d : bomber_game_->getDynamicObject())
        if (d.getType() == BomberObject::P1_PLAYER)
            return false;
    return true;
}
Base* GamePlayState::update(GameContext* parent) {
    Base* next = this;
    Base* next_state = derived_->update(this);
    if (next_state != derived_) {
        DYNAMIC_DEL(derived_);
        auto casted = dynamic_cast<SubSubState*>( next_state);
        if (casted) {
            DYNAMIC_DEL(derived_);
            derived_ = casted;
        }
        else {
            next = next_state;
        }
        return next;
    }
    ASSERT(next_state != nullptr);
    return next;
}

