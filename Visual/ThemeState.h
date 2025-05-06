#pragma once
#include "RootState.h"
struct DDS;

class ThemeState:public GameState { // 主题状态
public:
    ThemeState();
    ~ThemeState();
    virtual GameState* update(GameContext* parent)override;
private:
    DDS* theme_img;
    int selection;
};