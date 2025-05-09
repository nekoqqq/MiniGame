#pragma once
#include <memory>
#include "GamePlayState.h"
struct DDS;
class GamePlayState;

class P1LoadingState:public SubSubState {
public:
    P1LoadingState();
    ~P1LoadingState();
    virtual Base* update(GamePlayState* parent)override;
private:
    DDS* loading_img;
    unsigned loading_start_time;
};

class PlayState:public SubSubState {
public:
    // 前处理部分
    void pre(GamePlayState* parent);
    // 后处理部分
    void post(GamePlayState* parent);
    PlayState();
    ~PlayState();
    virtual Base* update(GamePlayState* parent)override;
};

class MenuState:public SubSubState {
public:
    MenuState();
    ~MenuState();
    virtual Base* update(GamePlayState* parent)override;
private:
    DDS* menu_img;
    int cur_selection;
    const char* menu_str[2] = { "CONTINUE","BACK TO THEME"};
};

class FailedState:public SubSubState {
public:
    FailedState();
    ~FailedState();
    virtual Base* update(GamePlayState* parent)override;
private:
    DDS* failed_img;
    unsigned failed_start_time;
};

class SucceedState:public SubSubState { // 单人模式过关状态
public:
    SucceedState();
    virtual Base* update(GamePlayState* parent)override;
private:
    std::unique_ptr<DDS> succeed_img;
    unsigned succeed_start_time;
};

class OutcomeState:public SubSubState { // 双人模式胜负状态
public:
    OutcomeState();
    virtual Base* update(GamePlayState* parent)override;
private:
    std::unique_ptr<DDS> outcome_img;
    int selection;
};