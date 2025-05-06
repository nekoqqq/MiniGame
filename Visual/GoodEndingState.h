#pragma once
#include <memory>
#include "RootState.h"

struct DDS;

class GoodEndingState:public GameState { // 单人模式通关状态
public:
    GoodEndingState();
    ~GoodEndingState();
    virtual GameState* update(GameContext* parent)override;
private:
    std::unique_ptr<DDS> good_ending_img;
    unsigned good_ending_start_time;
};