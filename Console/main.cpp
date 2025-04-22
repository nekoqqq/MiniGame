//
//  main.cpp
//  MDGame
//
//  Created by qi on 2025/4/8.
// 推箱子游戏，用WASD控制方向并输入Enter确认，所有的箱子到达目标位置后游戏即结束
//
#include "Game.h"
#include "GameLib/Framework.h"
#include<iostream>
using namespace std;
namespace GameLib {
    void GameLib::Framework::update() {

    }
}

int main() {
    cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << endl;
    int stage = 3;
    ConsoleGame *gameState = new ConsoleGame(MapSource::FILE,stage);
    gameState->draw();
    char c;
    while (cin >> c) {
        gameState->set_input(c);
        gameState->update();
        gameState->draw();
        if (gameState->is_finished())
        {
            gameState->draw();
            cout << "YOU WIN! Total steps(exculude invalid steps): " << gameState->steps_ << "." << endl;
            exit(0);
        }
    }
}
