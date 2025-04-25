#pragma once
#include <memory>

class RootState;
struct DDS;

class GoodEndingState { // 单人模式通关状态
public:
    GoodEndingState();
    ~GoodEndingState();
    void update(RootState* parent);
private:
    std::unique_ptr<DDS> good_ending_img;
    unsigned good_ending_start_time;
};