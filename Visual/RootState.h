#pragma once
#include "GameLib/Framework.h"
class GamePlayState;

#ifndef PTR_DELETE
#define DYNAMIC_DEL( x ) { delete ( x ); ( x ) = nullptr;}
#endif // !PTR_DELETE


class SubState;

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

    SubState* current_state_;

    GameContext();
    ~GameContext();;
    GameContext(const GameContext&)=delete;
    GameContext& operator=(const GameContext&) = delete;
};

class Base {
public:
    virtual Base* update(Base* p) = 0;
    ~Base() = default;
};


class SubState:public Base {
public:
    virtual Base* update(GameContext*) = 0;
    virtual Base* update(Base* p) override;
    virtual ~SubState() = default;
};


class SubSubState :public Base {
public:
    virtual Base* update(GamePlayState* parent) = 0;
    virtual Base* update(Base* p)override;
    virtual ~SubSubState() = default;

};