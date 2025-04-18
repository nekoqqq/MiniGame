#pragma once
#include<iostream>
#include<string>
#include<utility>
#include<vector>
#include<fstream>
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
        BOX = 'o',
        PLAYER = 'p',
        TARGET = '.',
        BOUNDARY = '#',
        BLANK = ' ',
        BOX_READY = 'O', // 箱子正好在要推的地方
        PLAYER_HIT = 'P',  // 人站在要推的地方上
    };
    Type getType()const;

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
    explicit operator DDS&() const;
    friend ostream& operator<<(ostream & out,const GameObject &);
private:
    Type type;
    // DDS* p_dds; // 各种图片素材 TODO 加在每个对象里面
    // 从某个方向移动过来
    int move_dx; 
    int move_dy;
};

class Game {
public:
    enum DIRECTION {
        UP = 0,
        LEFT = 1,
        RIGHT = 2,
        DOWN = 3
    };
    Game();
    ~Game();

    void init(MapSource,bool); // 初始化游戏中的各种状态
    virtual void update(string&) = 0; // 更新游戏状态，当前主要功能为处理输入
    virtual void update() = 0; // 实时输入
    virtual void draw() = 0; // 画图
    bool is_finished()const; // 判断当前游戏是否已经结束
    int steps_; // 总共用的步数
    static DDS& getImg(IMG_TYPE);
    friend ostream& operator<<(ostream& out, Game& g);

protected:
    // 地图大小
    int height_;
    int width_;
    vector<vector<GameObject>> grid_obj;
    vector<vector<char>> grid_;
    vector<pair<int, int>> box_pos_; // 箱子位置
    vector<pair<int, int>> target_pos_; // 箱子的目标位置
    pair<int, int> player_pos_; // 玩家的位置
    bool _valid(pair<int, int>&)const; // 判断当前是否是有效的位置
    void _update_objects(int direction);

    int move_count=0;
    int var_move_count=0; // 可变FPS计数器
    const int MAX_VAR_MOVE_COUNT = 50; // 50 ms 根据时间移动相应的像素,50ms移动32个像素, 则速度为640px/s
    bool var_fps; // 是否是可变刷新率的游戏

private:
   static DDS* p_dds; // 各种图片素材, 无需定义为static, 因为Game对象理论上全局只有一个
};

class ConsoleGame :public Game {
public:
    virtual void update(string&) override;
    virtual void update();
    virtual void draw() override;
};