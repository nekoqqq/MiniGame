#pragma once
class HSMGame;
class P1LoadingState;
class PlayState;
class MenuState;
class RootState;
class SucceedState;
class FailedState;
class OutcomeState;

class MainState {
public:
    enum State {
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

    MainState(int stage);
    ~MainState();
    
    Mode getMode()const;

    void setState(State state);

    HSMGame* getHSMGame()const;
    void setStage(int);
    int getStage()const;
    bool isFinishAllStage()const;

    void setTrial(int);
    int getTrial()const;
    bool isFailed()const;
    void update(RootState* parent);

private:
    State state_;
    int stage_;
    const int TOTAL_STAGE = 3;
    int trial_;
    const int MAX_TRIAL = 3;

    HSMGame* hsm_game_;

    P1LoadingState* p1_loading_state_;
    PlayState* play_state_;
    MenuState* menu_state_;
    SucceedState* succeed_state_;
    FailedState* failed_state_;
    OutcomeState* outcome_state_;
};