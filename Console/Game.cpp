#include"Game.h"

DDS* Game::p_dds = new DDS[8]{ 
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\box.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\player.dds" ,
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\target.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\boundary.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\blank.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\box_ready.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\player_hit.dds",
    "C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\main_theme.dds",
};
Game::Game() :height_(0), width_(0), steps_(0) {}
Game::~Game() { delete []p_dds; p_dds = nullptr; }
bool Game::_valid(pair<int, int>& pos) const{
    if (pos.first >= 0 && pos.first < height_ && pos.second >= 0 && pos.second < width_ && grid_obj[pos.first][pos.second].getType() != GameObject::BOUNDARY)
        return true;
    return false;
}
void Game::_update_objects(int direction) {
    pair<int, int> one_step_pos, two_steps_pos;
    int dx = 0, dy = 0;
    if (direction == UP) // W
        dx = -1;
    else if (direction == LEFT) // A
        dy = -1;
    else if (direction == RIGHT) // S
        dx = 1;
    else if (direction == DOWN) // D
        dy = 1;
    else
        return;

    one_step_pos = make_pair(player_pos_.first + dx, player_pos_.second + dy);
    two_steps_pos = make_pair(one_step_pos.first + dx, one_step_pos.second + dy);
    if (!_valid(one_step_pos))
        return;

    GameObject & one_step_obj = grid_obj[one_step_pos.first][one_step_pos.second];
    GameObject & two_steps_obj = grid_obj[two_steps_pos.first][two_steps_pos.second];

    if (one_step_obj == GameObject::BOX || one_step_obj == GameObject::BOX_READY) { 
        if (!_valid(two_steps_pos) || two_steps_obj == GameObject::BOX || two_steps_obj == GameObject::BOX_READY) // 两个箱子推不动了
            return;
        if (two_steps_obj == GameObject::TARGET) // 正好可以箱子就绪
            grid_obj[two_steps_pos.first][two_steps_pos.second] = GameObject::BOX_READY;

        else // 普通的位置
            grid_obj[two_steps_pos.first][two_steps_pos.second] = GameObject::BOX;
        two_steps_obj.set_move(dx, dy); // 箱子动画
    }

    // 更新玩家位置,这里没有办法只能遍历判断初始的是空的还是目标
    move_count = 1;
    var_move_count = 1;
    one_step_obj.set_move(dx, dy); // 人物动画
    steps_++;

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

    return succeed >= box_pos_.size();
}
DDS& Game::getImg(IMG_TYPE img_type)
{
    int index = img_type;
    return p_dds[index];
}
void Game::init(MapSource mapSource,bool var_fps) {
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
        ifstream fin("C:/Users/colorful/source/repos/MiniGame/Console/map.txt", ios_base::in);
        if (!fin.is_open()) {
            cout << "打开失败" << endl;
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

        height_ = grid_obj.size();

        // TODO：暂时不支持非等长等宽的形状
        for (int i = 0; i < grid_obj.size(); i++) {
            for (int j = 0; j < grid_obj[i].size(); j++) {
                if (grid_obj[i][j] == GameObject::BOX) {
                    box_pos_.push_back({ i,j });
                }
                else if (grid_obj[i][j] == GameObject::PLAYER)
                    player_pos_ = { i,j };
                else if (grid_obj[i][j] == GameObject::TARGET)
                    target_pos_.push_back({ i,j });

            }
            width_ = max(width_, (int)grid_obj[i].size());
        }
    }
    this->var_fps = var_fps;

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
GameObject::Type GameObject::getType()const { return type; }
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
GameObject::operator DDS&() const {
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
    return Game::getImg(img_type);
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

void ConsoleGame::update(){}
void ConsoleGame::update(string& input) {
    int direction;
    for (int i = 0; i < input.size(); i++) {
        switch (input[i]) {
        case 'w':
        case 'W':
            direction = 0;
            break;
        case 'a':
        case 'A':
            direction = 1;
            break;
        case 's':
        case 'S':
            direction = 2;
            break;
        case 'd':
        case 'D':
            direction = 3;
            break;
        default:
            break;
        }
        _update_objects(direction);
        if (is_finished())
        {
            draw();
            cout << "YOU WIN! Total steps(exculude invalid steps): " << steps_ << "." << endl;
            exit(0);
        }
    }
}
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
