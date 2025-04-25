#pragma once
class ThemeState;
class SelectionState;
class MainState;
class GoodEndingState;
class BadEndingState;

#ifndef PTR_DELETE
#define DYNAMIC_DEL( x ) { delete ( x ); ( x ) = nullptr;}
#endif // !PTR_DELETE

class RootState {
public:
    enum State {
        UNKNOW,
        THEME, // 主题
        MAIN, // 主要游戏状态
        GOOD_ENDING, // 通关状态
        BAD_ENDING // 失败状态
    };

    enum Mode {
        UNK,
        P1, // 单人游戏
        P2 // 双人游戏
    };
    static RootState& instance();

    void update();
    void setState(State state);
    void setMode(Mode);
    Mode getMode()const;

private:
    State state_;
    Mode mode_;

    ThemeState* theme_state_;
    MainState* main_state_;
    GoodEndingState* good_ending_state_;
    BadEndingState* bad_ending_state_;

    RootState();
    ~RootState();
    RootState(const RootState&) = delete;
    RootState& operator=(const RootState&) = delete;
};