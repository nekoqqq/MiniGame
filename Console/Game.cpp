#include"Game.h"


Game::Game() :Game(MapSource::FILE,1,true) {}
Game::Game(MapSource mapSource, bool var_fps,int stage):stage(stage){ init(mapSource,var_fps); }
Game::~Game() { delete[] p_dds; p_dds = nullptr; grid_obj.clear(); box_pos_.clear(); target_pos_.clear(); }
bool Game::_valid(pair<int, int>& pos) const{
    if (pos.first >= 0 && pos.first < height_ && pos.second >= 0 && pos.second < width_ && grid_obj[pos.first][pos.second].getType() != GameObject::BOUNDARY)
        return true;
    return false;
}
void Game::update() {
       preHandle();
       DIRECTION direction = handleInput();
       pair<int, int> delta = updatePosition(direction);
       updateState(delta);
       moveObject(delta);
}


void Game::preHandle() { }
Game::DIRECTION Game::handleInput() { return UNKNOW; }
pair<int,int> Game::updatePosition(DIRECTION direction) {
    pair<int, int> one_step_pos, two_steps_pos;
    int dx = 0, dy = 0;
    if (direction == UP) // W
        dx = -1;
    else if (direction == LEFT) // A
        dy = -1;
    else if (direction == DOWN) // S
        dx = 1;
    else if (direction == RIGHT) // D
        dy = 1;
    else
        return { 0,0 };

    one_step_pos = make_pair(player_pos_.first + dx, player_pos_.second + dy);
    two_steps_pos = make_pair(one_step_pos.first + dx, one_step_pos.second + dy);
    if (!_valid(one_step_pos))
        return { 0,0 };

    GameObject& one_step_obj = grid_obj[one_step_pos.first][one_step_pos.second];
    GameObject& two_steps_obj = grid_obj[two_steps_pos.first][two_steps_pos.second];

    if (one_step_obj == GameObject::BOX || one_step_obj == GameObject::BOX_READY) {
        if (!_valid(two_steps_pos) || two_steps_obj == GameObject::BOX || two_steps_obj == GameObject::BOX_READY) // 两个箱子推不动了
            return { 0,0 };
        if (two_steps_obj == GameObject::TARGET) // 正好可以箱子就绪
            grid_obj[two_steps_pos.first][two_steps_pos.second] = GameObject::BOX_READY;
        else // 普通的位置
            grid_obj[two_steps_pos.first][two_steps_pos.second] = GameObject::BOX;
        two_steps_obj.set_move(dx, dy); // 箱子动画
    }
    return { dx,dy };
}
void Game::updateState(pair<int,int> &delta) {}
void Game::moveObject(pair<int,int> &delta) {
    if (delta.first == 0 && delta.second == 0)
        return;
    int dx = delta.first;
    int dy = delta.second;
    steps_++;
    
    pair<int,int> one_step_pos = make_pair(player_pos_.first + dx, player_pos_.second + dy);
    GameObject& one_step_obj = grid_obj[one_step_pos.first][one_step_pos.second];

    one_step_obj.set_move(dx, dy);

    for (auto& t : target_pos_)
        if (player_pos_.first == t.first && player_pos_.second == t.second) {
            grid_obj[player_pos_.first][player_pos_.second] = GameObject::TARGET;
            break;
        }
        else
            grid_obj[player_pos_.first][player_pos_.second] = GameObject::BLANK;

    player_pos_.first += dx;
    player_pos_.second += dy;

    // 根据玩家站的位置确定状态
    if (one_step_obj == GameObject::TARGET || one_step_obj == GameObject::BOX_READY) // 修复一个bug，玩家当前面对的是就绪的箱子或者目标位置，都应该设置为人物在箱子上面
        grid_obj[player_pos_.first][player_pos_.second] = GameObject::PLAYER_HIT;
    else
        grid_obj[player_pos_.first][player_pos_.second] = GameObject::PLAYER;
}


bool Game::is_finished()const{
    int succeed = 0;
    for (auto& t : target_pos_)
        if (grid_obj[t.first][t.second] == GameObject::BOX_READY)
            succeed += 1;
    bool finished = succeed >= box_pos_.size();
    return finished;
}
DDS& Game::getImg(IMG_TYPE img_type)
{
    int index = img_type;
    return p_dds[index];
}
void Game::loadFile(string file_name) {
    ifstream fin(file_name, ios_base::in);
    if (!fin.is_open()) {
        cout <<file_name<< "打开失败" << endl;
        exit(-1);
    }
    string line;
    int i = 0;
    while (getline(fin, line)) {
        grid_obj.push_back(vector<GameObject>(line.size()));
        for (int j = 0; j < line.size(); j++)
            grid_obj[i][j] = line[j];
        i++;
    }
}
int Game::getWidth() const
{
    return width_;
}
GameObject& Game::getGameObject(int i, int j)
{
    return grid_obj[i][j];
}
int Game::getHeight() const
{
    return height_;
}
void Game::init(MapSource mapSource,bool var_fps)       {
    if (mapSource == MapSource::PREDEFINED) {
        height_ = 5;
        width_ = 8;
        box_pos_ = { {2,2},{2,5} };
        target_pos_ = { {1,2},{1,3},{3,5} };
        player_pos_ = { 1,5 };

        grid_obj.resize(height_);
        for (int i = 0; i < grid_obj.size(); i++)
            grid_obj[i].resize(width_);

        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_; j++) {
                if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
                    grid_obj[i][j] = GameObject::BOUNDARY;
                else
                    grid_obj[i][j] = GameObject::BLANK;
            }
        }

        // set box
        for (auto& pos : box_pos_)
            grid_obj[pos.first][pos.second] = GameObject::BOX;

        // set target
        for (auto& pos : target_pos_)
            grid_obj[pos.first][pos.second] = GameObject::TARGET;


        // set player
        grid_obj[player_pos_.first][player_pos_.second] = GameObject::PLAYER;
    }
    else if (mapSource == MapSource::FILE) {
        string file_name = "C:/Users/colorful/source/repos/MiniGame/Console/stage/stage" + to_string(stage) + ".txt"; // stage1.txt
        loadFile(file_name);

        assert(grid_obj.size() > 0 && "Grid Object初始化成功");
        height_ = grid_obj.size();

        // TODO：暂时不支持非等长等宽的形状
        for (int i = 0; i < grid_obj.size(); i++) {
            for (int j = 0; j < grid_obj[i].size(); j++) {
                if (grid_obj[i][j] == GameObject::BOX) {
                    box_pos_.push_back({ i,j });
                }
                else if (grid_obj[i][j] == GameObject::PLAYER || grid_obj[i][j]==GameObject::PLAYER_HIT)
                    player_pos_ = { i,j };
                else if (grid_obj[i][j] == GameObject::TARGET || grid_obj[i][j]==GameObject::PLAYER_HIT)
                    target_pos_.push_back({ i,j });

            }
            width_ = max(width_, (int)grid_obj[i].size());
        }

        for (int i = 0; i < height_; i++) {
            for (int j = grid_obj[i].size(); j < width_; j++)
                grid_obj[i].push_back(GameObject(GameObject::BOUNDARY));
        }

    }
    this->var_fps = var_fps;
    this->finished = false;

    p_dds = new DDS[7]{
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\box.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\player.dds" ,
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\target.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\boundary.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\blank.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\box_ready.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\player_hit.dds"
    };
}
bool Game::isGameVar()const {
    return var_fps;
}

void GameObject::set_type(Type type) {
    this->type = type;
}
void GameObject::set_move(int dx, int dy) {
    move_dx = dx;
    move_dy = dy;
}
pair<int,int> GameObject::get_move() {
    return { move_dx,move_dy };
}
GameObject::GameObject():GameObject(UNKNOW,0,0){} // c++11 委托构造函数, 并且使用条款04：确定对象使用前已先被初始化
GameObject::GameObject(Type type):GameObject(type,0,0){}
GameObject::GameObject(Type type, int move_dx, int move_dy) :type(type), move_dx(0), move_dy(0) {}
GameObject::Type GameObject::getType()const { return type; }
IMG_TYPE GameObject::getImgType() const
{
    IMG_TYPE img_type = IMG_BLANK;
    switch (this->type)
    {
    case BOX:
        img_type = IMG_BOX;
        break;
    case PLAYER:
        img_type = IMG_PLAYER;
        break;
    case TARGET:
        img_type = IMG_TARGET;
        break;
    case BOUNDARY:
        img_type = IMG_BOUNDARY;
        break;
    case BLANK:
        img_type = IMG_BLANK;
        break;
    case BOX_READY:
        img_type = IMG_BOX_READY;
        break;
    case PLAYER_HIT:
        img_type = IMG_PLAYER_HIT;
        break;
    default:
        break;
    }
    return img_type;
}
bool GameObject::operator==(const GameObject &other)const {
    return this->type == other.getType() && this->move_dx == other.move_dx && this->move_dy == other.move_dy;
}
bool GameObject::operator!=(const GameObject& other)const {
    return !(*this == other);
}
bool GameObject::operator==(Type type)const {
    return this->type == type;
}
bool GameObject::operator!=(Type type)const {
    return !(*this == type);
}
GameObject& GameObject::operator=(Type type) {
    this->type = type;
    return *this;
}
GameObject& GameObject::operator=(char c)
{
    this->type = static_cast<Type> (c);
    return *this;
}
GameObject::operator char()const {
    return (char)this->type;
}
ostream& operator<<(ostream& out, const GameObject&go) {
    return out << (char)go.getType();
}
ostream& operator<<(ostream& out, Game& g)
{
    for (int i = 0; i < g.grid_obj.size(); i++,out<<endl)
        for (int j = 0; j < g.grid_obj[i].size(); j++)
            out << g.grid_obj[i][j] ;
    out << endl;
    return out;
}

Game::DIRECTION ConsoleGame::handleInput() {
    DIRECTION direction = UNKNOW;
    switch (c) {
    case 'w':
    case 'W':
        direction = UP;
        break;
    case 'a':
    case 'A':
        direction = LEFT;
        break;
    case 's':
    case 'S':
        direction = DOWN;
        break;
    case 'd':
    case 'D':
        direction = RIGHT;
        break;
    default:
        break;
    }
    return direction;
}
void ConsoleGame::set_input(char c)
{
    this->c = c;
}
ConsoleGame::ConsoleGame(MapSource mapSource, int stage) :Game(mapSource, false, stage) ,c('0'){ }
void ConsoleGame::draw() {
    cout << *this;
}


DDS::DWORD* DDS::get_image_data()
{
    return dData;
}
DDS::DWORD DDS::get_image_width() const
{
    return dWidth;
}
DDS::DWORD DDS::get_image_height() const
{
    return dHeight;
}
