#pragma once
class DDS;
class MainState;
class LoadingState {
public:
    LoadingState();
    ~LoadingState();
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
    const char* menu_str[5] = { "<MENU SELECT>","RETRY","BACK TO SELECT","BACK TO TITLE","CONTINUE" };
    const int menu_size = sizeof(menu_str) / sizeof(const char*);
};

class EndingState {
public:
    EndingState();
    ~EndingState();
    void update(MainState* parent);
private:
    DDS* ending_img;
    unsigned ending_start_time;
};