//
//  main.cpp
//  MDGame
//
//  Created by qi on 2025/4/8.
// 推箱子游戏，用WASD控制方向并输入Enter确认，所有的箱子到达目标位置后游戏即结束
//

#include "Game.h"
struct DDSD { // DirectX格式图片
    using DWORD = unsigned long;
    using BYTE = unsigned char;

    DWORD dwSize = 124; // 结构体大小，固定为124字节
    DWORD dwFlags; // 标志位信息
    DWORD dwHeight; // 图片高度，位于8开始的4字节
    DWORD dwWidth; // 图片宽度，位于12开始的4字节
    DWORD dwPitchOrLinearSize; // 一行的字节数
    DWORD dwDepth; // 3D纹理深度
    DWORD dwMipMapCount; // MipMap的数量
    DWORD dwReserved1[11]; // 其他信息
    DWORD dwReserved2; // 未使用信息

    DDSD(const char* file_name ) {
        ifstream file(file_name, ios_base::binary);
        if (!file.is_open()) {
            cout << "读取文件失败" << endl;
            file.close();
            return;
        }
        file.seekg(0, ios_base::end);
        cout<<file.tellg();

        file.seekg(8,ios_base::beg);
        file >> dwHeight;
        file >> dwWidth;

        cout << dwHeight << " " << dwWidth << endl;
        file.close();
    }
};



int main() {
    cout << "Welcome to my game, please press keyboard W|A|S|D for UP|LEFT|RIGHT|DOWN." << endl;
    Game *gameState = new ConsoleGame();
    MAPSOURCE mapSource = MAPSOURCE::PREDEFINED;
    gameState->init(mapSource);
    gameState->draw();
    string input;
    while (cin >> input) {
        gameState->update(input);
        gameState->draw();
    }
}
