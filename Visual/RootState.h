#pragma once
class ThemeState;
class SelectionState;
class MainState;

#ifndef PTR_DELETE
#define DYNAMIC_DEL( x ) { delete ( x ); ( x ) = nullptr;}
#endif // !PTR_DELETE

class RootState {
public:
    enum State {
        UNKNOW,
        THEME,
        SELECTION,
        MAIN
    };

    RootState();
    ~RootState();
    void update();
    void setState(State state);
    void setStage(int stage) { stage_ = stage; }

private:
    State state_;
    int stage_;

    ThemeState* theme_state_;
    SelectionState* selection_state_;
    MainState* main_state_;
};