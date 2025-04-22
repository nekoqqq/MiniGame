/*
* 使用层次状态机模型实现
* RootSate
* |
* ThemeState SelectionState MainState
*                           |
*                           LoadingState PlayState MenuState EndingState
*/
#pragma once
#include<cassert>
#include "VisualGame.h"
#include "GameLib/Framework.h"
#include "../Console/Game.h"
#include "../Console/DDS.h"
using namespace GameLib;

class HSMGame :public VisualGame {
public:
    HSMGame(int, bool);
    unsigned getLastFrameCost();
    void updateStat(unsigned t);
    static const int FPS = 60;
    static unsigned previous_time[FPS];
};