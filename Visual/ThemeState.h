#pragma once
#include "RootState.h"
struct DDS;

class ThemeState:public SubState { // 主题状态
public:
    ThemeState();
    ~ThemeState();
    virtual Base* update(GameContext* parent)override;
private:
    DDS* theme_img;
    int selection;
};