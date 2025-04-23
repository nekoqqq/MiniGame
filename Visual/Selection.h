#pragma once
class RootState;
class DDS;

class SelectionState {
public:
    SelectionState();
    ~SelectionState();
    void update(RootState* parent);
private:
    DDS* selection_img;
    int cur_stage;
};