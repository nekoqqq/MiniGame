#pragma once
#include <memory>
struct DDS;
class RootState;

class BadEndingState { // 单人模式失败状态
public:
    BadEndingState();
    ~BadEndingState(); // 直接将析构函数=default写在这里还是会报错
    void update(RootState* parent);
private:
    std::unique_ptr<DDS> failure_img;
    unsigned failure_start_time;
};