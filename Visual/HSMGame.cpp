#include "HSMGame.h"
unsigned HSMGame::previous_time[FPS];
HSMGame::HSMGame(int stage, bool var_fps):VisualGame(stage,var_fps)
{
}
unsigned HSMGame::getLastFrameCost() {
    return previous_time[FPS - 1];
}
void HSMGame::updateStat(unsigned t) { // 更新内部的统计量
    for (int i = 0; i < FPS - 1; i++)
        previous_time[i] = previous_time[i + 1];
    previous_time[FPS - 1] = t;
}
