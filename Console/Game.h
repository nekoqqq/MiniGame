#pragma once
#include<iostream>
#include<string>
#include<utility>
#include<vector>
#include<fstream>
using namespace std;


// 地图来源，自定义或者从文件中
enum class MAPSOURCE {
    PREDEFINED, // 程序内部预定义
    FILE // 文件内
};


class Game {
public:
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
    Game();

    virtual void init(MAPSOURCE) = 0; // 初始化游戏中的各种状态
    virtual void update(string&) = 0; // 更新游戏状态，当前主要功能为处理输入
    virtual void draw() = 0; // 画图

protected:
    // 地图大小
    int height_;
    int width_;
    vector<vector<char>> grid_;
    int steps_; // 总共用的步数
    vector<pair<int, int>> box_pos_; // 箱子位置
    vector<pair<int, int>> target_pos_; // 箱子的目标位置
    pair<int, int> player_pos_; // 玩家的位置

    bool win()const; // 判断当前游戏是否已经结束
    bool valid(pair<int, int>&)const; // 判断当前是否是有效的位置
    void _update_objects(pair<int, int>& new_pos, int direction);
};

class ConsoleGame :public Game {
public:
    virtual void init(MAPSOURCE) override;
    virtual void update(string&) override;
    virtual void draw() override;
};
