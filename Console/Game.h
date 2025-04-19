#pragma once
#include<iostream>
#include<string>
#include<utility>
#include<vector>
#include<fstream>
#include<cassert>
using namespace std;


// 地图来源，自定义或者从文件中
enum class MapSource {
    PREDEFINED, // 程序内部预定义
    FILE, // 文件内
    VISUAL // 窗口大小
};

enum IMG_TYPE {
    IMG_BOX,
    IMG_PLAYER,
    IMG_TARGET,
    IMG_BOUNDARY,
    IMG_BLANK,
    IMG_BOX_READY,
    IMG_PLAYER_HIT,
    IMG_THEME // 主题
};

struct DDS { // DirectX格式图片
    using DWORD = unsigned;
    using BYTE = unsigned char;

    const char dMagic[4] = { 'D','D','S',' ' }; // 文件头，固定为"DDS "
    DWORD dSize = 124; // 结构体大小，固定为124字节
    DWORD dFlags; // 标志位信息
    DWORD dHeight; // 图片高度，位于12开始的4字节
    DWORD dWidth; // 图片宽度，位于16开始的4字节
    DWORD dPitchOrLinearSize; // 一行的字节数
    DWORD dDepth; // 3D纹理深度
    DWORD dMipMapCount; // MipMap的数量
    DWORD dReserved1[11]; // 其他信息
    DWORD dReserved2; // 未使用信息
    DWORD* dData; // 图片数据


    DDS(const char* file_name) {
        ifstream file(file_name, ios_base::binary);
        if (!file.is_open()) {
            std::cout << file_name << " " << "文件打开失败" << endl;
            file.close();
            throw exception();
        }
        // 读取文件元信息
        file.seekg(sizeof(dMagic) + sizeof(dSize) + sizeof(dFlags), ios_base::beg); //跳过前面的字节
        auto read_dword = [&file]() mutable {
            char s[sizeof(DWORD)];
            file.read(s, sizeof(DWORD));
            unsigned res = 0;
            for (int i = 0; i < sizeof(DWORD); i++)
                res |= static_cast<unsigned char>(s[i]) << (i * 8);
                
            return res;
            }; // 读取unsigned
        dHeight = read_dword();
        dWidth = read_dword();
        std::cout << file_name << " " << "读取的文件大小为: " << dHeight << "x" << dWidth << endl;
        // 读取图片数据
        file.seekg(sizeof(dMagic) + dSize, ios_base::beg);
        dData = new DWORD[dWidth * dHeight];
        for (int i = 0; i < dWidth * dHeight; i++)
            dData[i] = read_dword();
        file.close();
    }
    ~DDS() {
        if (dData) {
            delete[] dData;
            dData = nullptr;
        }
    }

    DWORD* get_image_data();
    DWORD get_image_width()const;
    DWORD get_image_height()const;

    DWORD& operator[](int i) { // 允许修改元素    
        return dData[i];
    }
};

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
    explicit GameObject(Type, int,int);

    Type getType()const;
    IMG_TYPE getImgType()const;

    void set_type(Type);
        

    void set_move(int dx, int dy);
    pair<int, int> get_move();

    bool operator==(const GameObject&)const;
    bool operator!=(const GameObject&)const;

    bool operator == (Type)const;
    bool operator !=(Type) const;

    GameObject& operator=(Type);
    GameObject& operator=(char);

    // 方便输出
    explicit operator char()const;  // 显示类型转换   
    friend ostream& operator<<(ostream & out,const GameObject &);
private:
    Type type;
    // 从某个方向移动过来
    int move_dx; 
    int move_dy;
};

enum class GameState {
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

    void init(MapSource,bool); // 初始化游戏中的各种状态
    void update(); // 实时输入, 根据条款35：考虑virtual函数以外的其他选择，这里使用NVI（non-virtual interface）的方式实现Template Method，不确定这个是不是完整的设计模式

    virtual void draw() = 0; // 画图
    bool is_finished()const; // 判断当前游戏是否已经结束
    void reset_game(MapSource, bool);
    int steps_; // 总共用的步数
    DDS& getImg(IMG_TYPE);
    friend ostream& operator<<(ostream& out, Game& g);
    void loadFile(string);

    int getHeight()const;
    int getWidth()const;
    GameObject& getGameObject(int i, int j);
    bool isGameVar()const;

protected:
    bool _valid(pair<int, int>&)const; // 判断当前是否是有效的位置
    
private:
   int stage; // 关卡
   bool finished = false;
   bool var_fps; // 固定帧率还是动态的游戏
   DDS* p_dds; // 各种图片素材, 无需定义为static, 因为Game对象理论上全局只有一个

   // 地图大小
   int height_;
   int width_;
   vector<vector<GameObject>> grid_obj;
   vector<pair<int, int>> box_pos_; // 箱子位置
   vector<pair<int, int>> target_pos_; // 箱子的目标位置
   pair<int, int> player_pos_; // 玩家的位置

   virtual void preHandle(); // 输入前处理
   virtual DIRECTION handleInput(); // 输入处理
   virtual pair<int, int> updatePosition(DIRECTION); // 根据输入得到需要移动的
   virtual void updateState(pair<int,int>&); // 额外的变量处理
   virtual void moveObject(pair<int,int>&); // 实际移动物体
};


class ConsoleGame :public Game {
public:
    ConsoleGame(MapSource,int);
    virtual void draw();
    void set_input(char);
private:
    char c; // 每次输入

    virtual DIRECTION handleInput(); // 只重写了这个方法    
};