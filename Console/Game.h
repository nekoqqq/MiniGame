#pragma once
#include "common_enum.h"
#include<vector>
#include<string>

struct DDS;
class GameObject;


// 地图来源，自定义或者从文件中
enum class MapSource {
    PREDEFINED, // 程序内部预定义
    FILE, // 文件内
    VISUAL // 窗口大小
};

enum class GameState {
    UNKONW, // 无效状态
    THEME, // 主题界面
    SELECTION, // 选关
    LOADING, // 加载
    GAME, // 游戏界面
    MENU, //菜单画面
    ENDING, // 通关
};

class Game {
public:
    enum DIRECTION {
        UNKNOW=0,
        UP = 1,
        LEFT = 2,
        RIGHT = 3,
        DOWN = 4
    };
    Game();
    Game(MapSource,bool,int);
    virtual ~Game();

    void init(); // 初始化游戏中的各种状态
    void reset(); // 重置游戏
    void update(); // 实时输入, 根据条款35：考虑virtual函数以外的其他选择，这里使用NVI（non-virtual interface）的方式实现Template Method，不确定这个是不是完整的设计模式
    virtual void draw() = 0; // 画图

    virtual bool is_finished()const; // 判断当前游戏是否已经结束
    int steps_; // 总共用的步数
    DDS& getImg(IMG_TYPE);
    friend std::ostream& operator<<(std::ostream& out, Game& g);
    void setStage(int);
    int  getStage()const;
    virtual void loadGame(int);
    void loadFile(std::string);

    int getHeight()const;
    int getWidth()const;
    GameObject& getGameObject(int i, int j);
    bool isGameVar()const;

protected:
    bool _valid(std::pair<int, int>&)const; // 判断当前是否是有效的位置

private:
    MapSource map_source;
    int stage; // 关卡
    bool finished = false;
    bool var_fps; // 固定帧率还是动态的游戏
    DDS* p_dds; // 各种图片素材, 无需定义为static, 因为Game对象理论上全局只有一个
    char c; // 输入，只针对Console的游戏有效

    // 地图大小
    int height_;
    int width_;
    std::vector<std::vector<GameObject>> grid_obj;
    std::vector<std::pair<int, int>> box_pos_; // 箱子位置
    std::vector<std::pair<int, int>> target_pos_; // 箱子的目标位置
    std::pair<int, int> player_pos_; // 玩家的位置

    virtual bool preHandle(); // 输入前处理
    virtual DIRECTION handleInput(); // 输入处理
    virtual std::pair<int, int> updatePosition(DIRECTION); // 根据输入得到需要移动的
    virtual void updateState(std::pair<int,int>&); // 额外的变量处理
    virtual void moveObject(std::pair<int,int>&); // 实际移动物体
};


class ConsoleGame :public Game {
public:
    ConsoleGame(MapSource,int);
    virtual void draw() override;
    void set_input(char);
private:
    char c;
    virtual DIRECTION handleInput() override; // 只重写了这个方法    
};