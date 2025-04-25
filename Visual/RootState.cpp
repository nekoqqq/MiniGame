#include <cassert>
#include "RootState.h"
#include "ThemeState.h"
#include "MainState.h"
#include "GoodEndingState.h"
#include "BadEndingState.h"
#include "GameLib/Framework.h"


RootState& RootState::instance()
{
    static RootState instance_;
    return instance_;
}


RootState::RootState() : state_(THEME),mode_(P1), theme_state_(new ThemeState()), main_state_(nullptr),good_ending_state_(nullptr),bad_ending_state_(nullptr) {}
RootState::~RootState()
{
    // 这里会不会有重复delete的问题？有重复delete，但是因为每次delete之后将指针置为nullptr，nullptr可以重复delete
    DYNAMIC_DEL(theme_state_);
    DYNAMIC_DEL(main_state_);
    DYNAMIC_DEL(good_ending_state_);
    DYNAMIC_DEL(bad_ending_state_);
}
void RootState::setState(State state) {
    state_ = state;
}

void RootState::setMode(Mode mode)
{
    mode_ = mode;
}

RootState::Mode RootState::getMode()const {
    return mode_;
}

void RootState::update() {
    switch (state_)
    {
        case THEME:
            ASSERT(
                (theme_state_ && !(main_state_ || good_ending_state_ || bad_ending_state_))  // 初次启动或者不动
                || 
                (main_state_ && !(theme_state_ || good_ending_state_ || bad_ending_state_)) 
                ||
                (good_ending_state_ && !(main_state_ || theme_state_ || bad_ending_state_))
                ||
                (bad_ending_state_ && !(main_state_ || theme_state_ || good_ending_state_))
                ); 

            if (!theme_state_ && main_state_) {
                DYNAMIC_DEL(main_state_);
                theme_state_ = new ThemeState();
            }
            else if (!theme_state_ && good_ending_state_) {
                DYNAMIC_DEL(good_ending_state_);
                theme_state_ = new ThemeState();
            }
            else if (!theme_state_ && bad_ending_state_) {
                DYNAMIC_DEL(bad_ending_state_);
                theme_state_ = new ThemeState();
            }
            theme_state_->update(this);
            break;
        case MAIN:
            ASSERT(
                (theme_state_ && !(main_state_ || good_ending_state_ || bad_ending_state_))
                ||
                (main_state_ &&!(theme_state_ || good_ending_state_ || bad_ending_state_))
            );
            if (!main_state_ && theme_state_) {
                DYNAMIC_DEL(theme_state_);
                main_state_ = new MainState(1); // 从第一关开始
            }
            main_state_->update(this);
            break;
        case GOOD_ENDING:
            ASSERT(
                (main_state_ && !(theme_state_ || bad_ending_state_ || good_ending_state_)) 
                ||
                (good_ending_state_ && !(theme_state_ || main_state_ || bad_ending_state_))
            );
            if (!good_ending_state_ && main_state_) {
                DYNAMIC_DEL(main_state_);
                good_ending_state_ = new GoodEndingState();
            }
            good_ending_state_->update(this);
            break;
        case BAD_ENDING:
            ASSERT(
                (main_state_ && !(theme_state_ || bad_ending_state_ || good_ending_state_)) 
                ||
                (bad_ending_state_ && !(theme_state_ || main_state_ || good_ending_state_))
            );
            if (!bad_ending_state_&& main_state_) {
                DYNAMIC_DEL(main_state_);
                bad_ending_state_ = new BadEndingState();
            }
            bad_ending_state_->update(this);
            break;
        case UNKNOW:
            HALT("不可能的状态!");
            break;
    }
}

