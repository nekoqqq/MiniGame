#pragma once
#include <memory>
struct DDS;
class MainState;
class P1LoadingState {
public:
    P1LoadingState();
    ~P1LoadingState();
    void update(MainState* parent);
private:
    DDS* loading_img;
    unsigned loading_start_time;
};

class PlayState {
public:
    // 前处理部分
    void pre(MainState* parent);
    // 后处理部分
    // 后处理部分
    void post(MainState* parent);
    void update(MainState* parent);
};

class MenuState {
public:
    MenuState();
    ~MenuState();
    void update(MainState* parent);
private:
    DDS* menu_img;
    int cur_selection;
    const char* menu_str[2] = { "CONTINUE","BACK TO THEME"};
};

class FailedState {
public:
    FailedState();
    ~FailedState();
    void update(MainState* parent);
private:
    DDS* failed_img;
    unsigned failed_start_time;
};

class SucceedState { // 单人模式过关状态
public:
    SucceedState();
    void update(MainState* parent);
private:
    std::unique_ptr<DDS> succeed_img;
    unsigned succeed_start_time;
};

class OutcomeState { // 双人模式胜负状态
public:
    OutcomeState();
    void update(MainState* parent);
private:
    std::unique_ptr<DDS> outcome_img;
    int selection;
};