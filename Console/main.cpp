//
//  main.cpp
//  MDGame
//
//  Created by qi on 2025/4/8.
// 推箱子游戏，用WASD控制方向并输入Enter确认，所有的箱子到达目标位置后游戏即结束
//
#include "Game.h"

int main() {
    cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << endl;
    Game *gameState = new ConsoleGame();
    MapSource mapSource = MapSource::FILE;
    gameState->init(mapSource);
    gameState->draw();
    string input;
    while (cin >> input) {
        gameState->update(input);
        gameState->draw();
    }
}
