#pragma once
#include "common_enum.h"
#include<utility>
#include<fstream>

class GameObject {
public:
    enum Type {
        UNKNOW = 'x', // 未知状态
        BOX = 'o',
        PLAYER = 'p',
        TARGET = '.',
        BOUNDARY = '#',
        BLANK = ' ',
        BOX_READY = 'O', // 箱子正好在要推的地方
        PLAYER_HIT = 'P',  // 人站在要推的地方上
    };
    explicit GameObject();
    explicit GameObject(Type);
    explicit GameObject(Type, int, int);

    Type getType()const;
    IMG_TYPE getImgType()const;

    void set_type(Type);


    void set_move(int dx, int dy);
    std::pair<int, int> get_move();

    bool operator==(const GameObject&)const;
    bool operator!=(const GameObject&)const;

    bool operator == (Type)const;
    bool operator !=(Type) const;

    GameObject& operator=(Type);
    GameObject& operator=(char);

    virtual void drawAt(int, int)const;



    // 方便输出
    explicit operator char()const;  // 显示类型转换   
    friend std::ostream& operator<<(std::ostream& out, const GameObject&);
private:
    Type type;
    // 从某个方向移动过来
    int move_dx;
    int move_dy;
};
