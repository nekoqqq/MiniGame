#pragma once
#include <memory>
#include "RootState.h"
struct DDS;

class BadEndingState:public GameState { // 单人模式失败状态
public:
    BadEndingState();
    ~BadEndingState(); // 直接将析构函数=default写在这里还是会报错
    virtual GameState* update(GameContext* parent) override;
private:
    std::unique_ptr<DDS> failure_img;
    unsigned failure_start_time;
};