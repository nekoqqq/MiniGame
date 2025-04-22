#pragma once
class HSMGame;
class LoadingState;
class PlayState;
class MenuState;
class EndingState;
class RootState;

class MainState {
public:
    enum State {
        LOADING, // 加载
        PLAY, // 游戏过程
        MENU,
        ENDING, // 游戏结束

        // 可能的父状态
        THEME,
        SELECTION,
    };
    MainState(int stage);
    ~MainState();

    void setState(State state);

    HSMGame* getHSMGame()const;
    int getStage()const;

    void update(RootState* parent);
private:
    State state_;
    int stage_;
    HSMGame* hsm_game_;

    LoadingState* loading_state_;
    PlayState* play_state_;
    MenuState* menu_state_;
    EndingState* ending_state_;
};