#pragma once
#include <memory>
#include "GamePlayState.h"
struct DDS;
class GamePlayState;

class P1LoadingState:public MainState {
public:
    P1LoadingState();
    ~P1LoadingState();
    virtual MainState* update(GamePlayState* parent)override;
private:
    DDS* loading_img;
    unsigned loading_start_time;
};

class PlayState:public MainState {
public:
    // 前处理部分
    void pre(GamePlayState* parent);
    // 后处理部分
    void post(GamePlayState* parent);
    virtual MainState* update(GamePlayState* parent)override;
};

class MenuState:public MainState {
public:
    MenuState();
    ~MenuState();
    virtual MainState* update(GamePlayState* parent)override;
private:
    DDS* menu_img;
    int cur_selection;
    const char* menu_str[2] = { "CONTINUE","BACK TO THEME"};
};

class FailedState:public MainState {
public:
    FailedState();
    ~FailedState();
    virtual MainState* update(GamePlayState* parent)override;
private:
    DDS* failed_img;
    unsigned failed_start_time;
};

class SucceedState:public MainState { // 单人模式过关状态
public:
    SucceedState();
    virtual MainState* update(GamePlayState* parent)override;
private:
    std::unique_ptr<DDS> succeed_img;
    unsigned succeed_start_time;
};

class OutcomeState:public MainState { // 双人模式胜负状态
public:
    OutcomeState();
    virtual MainState* update(GamePlayState* parent)override;
private:
    std::unique_ptr<DDS> outcome_img;
    int selection;
};