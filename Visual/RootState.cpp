#include <cassert>
#include "RootState.h"
#include "ThemeState.h"
#include "MainState.h"
#include "Selection.h"
#include "GameLib/Framework.h"


RootState::RootState() : state_(THEME), stage_(0), theme_state_(new ThemeState()), selection_state_(nullptr), main_state_(nullptr) {}
RootState::~RootState()
{
    DYNAMIC_DEL(theme_state_);
    DYNAMIC_DEL(selection_state_);
    DYNAMIC_DEL(main_state_);
}
void RootState::update() {
    switch (state_)
    {
    case UNKNOW:
        HALT("不可能的状态!");
        break;
    case THEME:
        if (main_state_) {
            DYNAMIC_DEL(main_state_);
            theme_state_ = new ThemeState();
        }
        theme_state_->update(this);
        break;
    case SELECTION:
        if (theme_state_ || main_state_) {
            DYNAMIC_DEL(theme_state_);
            DYNAMIC_DEL(main_state_);
            selection_state_ = new SelectionState();
        }
        selection_state_->update(this);
        break;
    case MAIN:
        if (selection_state_) {
            DYNAMIC_DEL(selection_state_);
            assert(stage_ || "stage不合法!");
            main_state_ = new MainState(stage_);
        }
        main_state_->update(this);
        break;
    }
}
void RootState::setState(State state) {
    state_ = state;
}
