#pragma once
class ThemeState;
class SelectionState;
class GamePlayState;
class GoodEndingState;
class BadEndingState;

#ifndef PTR_DELETE
#define DYNAMIC_DEL( x ) { delete ( x ); ( x ) = nullptr;}
#endif // !PTR_DELETE

class GameContext;

class GameState {
public:
    virtual GameState* update(GameContext* root_context) = 0;
    virtual ~GameState()=default;
};

class GameContext {
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
    static GameContext& instance();

    void update();
    void setState(State state);
    void setMode(Mode);
    Mode getMode()const;

private:
    State state_;
    Mode mode_;

    GameState* current_state_;

    GameContext();
    ~GameContext() { delete current_state_; };
    GameContext(const GameContext&)=delete;
    GameContext& operator=(const GameContext&) = delete;
};