//
//  main.cpp
//  MDGame
//
//  Created by qi on 2025/4/8.
// 推箱子游戏，用WASD控制方向并输入Enter确认，所有的箱子到达目标位置后游戏即结束
//

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
using namespace std;

class Game {
private:
    // 地图大小
    int HEIGHT;
    int WIDTH;
    enum OBJECT {
        BOX = 'o',
        PLAYER = 'p',
        TARGET = '.',
        BOUNDARY = '#',
        BLANK = ' ',
    };

    enum DIRECTION {
        UP = 0,
        LEFT = 1,
        RIGHT = 2,
        DOWN = 3
    };
    vector<vector<char>> GRID;
    int steps; // 总共用的步数
    vector<pair<int, int>> box_pos; // 箱子位置
    vector<pair<int, int>> target_pos; // 箱子的目标位置
    pair<int, int> player_pos; // 玩家的位置

public:

    // 地图来源，自定义或者从文件中
    enum MAPSOURCE {
        PREDEFINED, // 程序内部预定义
        FILE // 文件内
    };

    void init(MAPSOURCE mapSource) {
        if (mapSource == PREDEFINED) {
            HEIGHT = 5;
            WIDTH = 8;
            box_pos = { {2,2},{2,5} };
            target_pos = { {1,2},{1,3},{3,5} };
            player_pos = { 1,5 };

            GRID.resize(HEIGHT);
            for (int i = 0; i < GRID.size(); i++)
                GRID[i].resize(WIDTH);

            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if (i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1)
                        GRID[i][j] = BOUNDARY;
                    else
                        GRID[i][j] = BLANK;
                }
            }

            // set box
            for (auto& pos : box_pos)
                GRID[pos.first][pos.second] = BOX;

            // set target
            for (auto& pos : target_pos)
                GRID[pos.first][pos.second] = TARGET;


            // set player
            GRID[player_pos.first][player_pos.second] = PLAYER;
        }
        else if (mapSource ==  FILE) {
            ifstream fin("map.txt", ios_base::in);
            if (!fin.is_open()) {
                cout << "打开失败" << endl;
                exit(-1);
            }
            string line;
            int i = 0;
            while (getline(fin, line)) {
                GRID.push_back(vector<char>(line.size()));
                for (int j = 0; j < line.size(); j++)
                    GRID[i][j] = line[j];
                i++;
            }

            HEIGHT = GRID.size();

            for (int i = 0; i < GRID.size(); i++) {
                for (int j = 0; j < GRID[i].size(); j++) {
                    if (GRID[i][j] == BOX) {
                        box_pos.push_back({ i,j });
                    }
                    else if (GRID[i][j] == PLAYER)
                        player_pos = { i,j };
                    else if (GRID[i][j] == TARGET)
                        target_pos.push_back({ i,j });

                }
                WIDTH = max(WIDTH, (int)GRID[i].size());
            }


        }
    }

    // 更新游戏状态，当前主要功能为处理输入
    virtual void  update(string& input) {
        pair<int, int> new_pos{ player_pos };
        pair<int, int> old_pos{ player_pos };
        for (int i = 0; i < input.size(); i++) {
            switch (input[i]) {
            case 'w':
            case 'W':
                _update_objects(new_pos, 0);
                break;
            case 'a':
            case 'A':
                _update_objects(new_pos, 1);
                break;
            case 's':
            case 'S':
                _update_objects(new_pos, 2);
                break;
            case 'd':
            case 'D':
                _update_objects(new_pos, 3);
                break;
            default:
                break;
            }
            if (new_pos != old_pos) {
                // 更新玩家位置
                steps++;
                GRID[new_pos.first][new_pos.second] = 'p';
                for (auto& t : target_pos)
                    if (old_pos.first == t.first && old_pos.second == t.second) {
                        GRID[old_pos.first][old_pos.second] = TARGET;
                        break;
                    }
                    else
                        GRID[old_pos.first][old_pos.second] = BLANK;
            }
            if (win())
            {
                draw();
                cout << "YOU WIN! Total steps(exculude invalid steps): "<<steps<<"." << endl;
                exit(0);
            }
            old_pos = new_pos;
        }
        player_pos = new_pos;
    } 
    
    // 画图
    virtual void draw() {
        for (int i = 0; i < GRID.size(); i++, cout << endl)
            for (int j = 0; j < GRID[i].size(); j++)
                cout << GRID[i][j];
    }


    // 判断当前是否是有效的位置
    bool valid(pair<int, int>& pos) {
        if (pos.first >= 0 && pos.first < HEIGHT && pos.second >= 0 && pos.second < WIDTH && GRID[pos.first][pos.second] != BOUNDARY)
            return true;
        return false;
    }

    // 判断当前游戏是否已经结束
    bool win() {
        int succeed = 0;
        for (auto& t : target_pos)
            if (GRID[t.first][t.second] == BOX)
                succeed += 1;

        return succeed >= box_pos.size();
    }

    void _update_objects(pair<int, int>& new_pos, int direction) {
        pair<int, int> box_target_pos;
        if (direction == UP) // W
        {
            new_pos.first -= 1;
            box_target_pos = make_pair(new_pos.first - 1, new_pos.second);
        }
        else if (direction == LEFT) // A
        {
            new_pos.second -= 1;
            box_target_pos = make_pair(new_pos.first, new_pos.second - 1);
        }
        else if (direction == RIGHT) // S
        {
            new_pos.first += 1;
            box_target_pos = make_pair(new_pos.first + 1, new_pos.second);
        }
        else if (direction == DOWN) // D
        {
            new_pos.second += 1;
            box_target_pos = make_pair(new_pos.first, new_pos.second + 1);
        }
        else
            throw exception();

        bool should_restore = false;
        if (valid(new_pos)) {
            if (GRID[new_pos.first][new_pos.second] == 'o') {
                if (valid(box_target_pos) && GRID[box_target_pos.first][box_target_pos.second] != BOX) { // 判断当前推的是否是箱子，如果是箱子继续往前推
                    GRID[box_target_pos.first][box_target_pos.second] = BOX;
                }
                else // 无法往前走
                    should_restore = true;
            }
        }
        else
            should_restore = true;

        if (should_restore)
            if (direction == 0) // W
                new_pos.first += 1;
            else if (direction == 1) // A
                new_pos.second += 1;
            else if (direction == 2) // S
                new_pos.first -= 1;
            else if (direction == 3) // D
                new_pos.second -= 1;
        return;
    }
};


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
    Game *gameState = new Game();
    Game::MAPSOURCE mapSource = Game::MAPSOURCE::FILE;
    gameState->init(mapSource);
    gameState->draw();
    string input;
    while (cin >> input) {
        gameState->update(input);
        gameState->draw();
    }
}
