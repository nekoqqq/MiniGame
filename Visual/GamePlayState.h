#pragma once
#include "RootState.h"
class HSMGame;
class BomberGame;


class GamePlayState:public SubState {
public:
    enum Phase {
        P1_LOADING, // 加载
        P2_LOADING, //双人游戏加载
        PLAY, // 游戏过程
        MENU,
        SUCCEED, // 通过当前关卡
        FAILED, // 失败当前关卡
        OUTCOME, // 游戏结果状态

        // 可能的父状态
        THEME,
        GOOD_ENDING,
        BAD_ENDING
    };
    enum Mode {
        UNK,
        P1, 
        P2
    };

    GamePlayState(int stage);
    ~GamePlayState();
    
    Mode getMode()const;

    void setState(Phase state);

    HSMGame* getHSMGame()const;
    BomberGame* getBomberGame() const;
    void setStage(int);
    int getStage()const;
    bool isFinishAllStage()const;

    void setTrial(int);
    int getTrial()const;
    bool isFailed()const;

    virtual Base* update(GameContext* parent) override;

private:
    Phase state_;
    int stage_;
    const int TOTAL_STAGE = 3;
    int trial_;
    const int MAX_TRIAL = 3;

    HSMGame* hsm_game_;
    BomberGame* bomber_game_;

    SubSubState* derived_;
};