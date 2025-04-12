#include"Game.h"

Game::Game() :height_(0), width_(0), steps_(0) {}
bool Game::_valid(pair<int, int>& pos) const{
    if (pos.first >= 0 && pos.first < height_ && pos.second >= 0 && pos.second < width_ && grid_[pos.first][pos.second] != BOUNDARY)
        return true;
    return false;
}
void Game::_update_objects(pair<int, int>& new_pos, int direction) {
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
        throw exception();

    one_step_pos = make_pair(new_pos.first + dx, new_pos.second + dy);
    two_steps_pos = make_pair(one_step_pos.first + dx, one_step_pos.second + dy);
    if (!_valid(one_step_pos))
        return;

    char one_step_obj = grid_[one_step_pos.first][one_step_pos.second];
    char two_steps_obj = grid_[two_steps_pos.first][two_steps_pos.second];
    if (one_step_obj == BOX || one_step_obj == BOX_READY) { 
        if (!_valid(two_steps_pos) || two_steps_obj == BOX || two_steps_obj == BOX_READY) // 两个箱子推不动了
            return;
        if (two_steps_obj == TARGET) // 正好可以箱子就绪
            grid_[two_steps_pos.first][two_steps_pos.second] = BOX_READY;
        else // 普通的位置
            grid_[two_steps_pos.first][two_steps_pos.second] = BOX;
    }

    // 更新玩家位置,这里没有办法只能遍历判断初始的是空的还是目标
    steps_++;
    for (auto& t : target_pos_)
        if (new_pos.first == t.first && new_pos.second == t.second) {
            grid_[new_pos.first][new_pos.second] = Game::OBJECT::TARGET;
            break;
        }
        else
            grid_[new_pos.first][new_pos.second] = Game::OBJECT::BLANK;

    new_pos.first += dx;
    new_pos.second += dy;

    // 根据玩家站的位置确定状态
    if (one_step_obj == TARGET || one_step_obj == BOX_READY) // 修复一个bug，玩家当前面对的是就绪的箱子或者目标位置，都应该设置为人物在箱子上面
        grid_[new_pos.first][new_pos.second] = PLAYER_HIT;
    else
        grid_[new_pos.first][new_pos.second] = PLAYER;
}
bool Game::is_finished()const{
    int succeed = 0;
    for (auto& t : target_pos_)
        if (grid_[t.first][t.second] == BOX_READY)
            succeed += 1;

    return succeed >= box_pos_.size();
}
void Game::init(MapSource mapSource){
    if (mapSource == MapSource::PREDEFINED) {
        height_ = 5;
        width_ = 8;
        box_pos_ = { {2,2},{2,5} };
        target_pos_ = { {1,2},{1,3},{3,5} };
        player_pos_ = { 1,5 };

        grid_.resize(height_);
        for (int i = 0; i < grid_.size(); i++)
            grid_[i].resize(width_);

        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_; j++) {
                if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
                    grid_[i][j] = BOUNDARY;
                else
                    grid_[i][j] = BLANK;
            }
        }

        // set box
        for (auto& pos : box_pos_)
            grid_[pos.first][pos.second] = BOX;

        // set target
        for (auto& pos : target_pos_)
            grid_[pos.first][pos.second] = TARGET;


        // set player
        grid_[player_pos_.first][player_pos_.second] = PLAYER;
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
            grid_.push_back(vector<char>(line.size()));
            for (int j = 0; j < line.size(); j++)
                grid_[i][j] = line[j];
            i++;
        }

        height_ = grid_.size();

        // TODO：暂时不支持非等长等宽的形状
        for (int i = 0; i < grid_.size(); i++) {
            for (int j = 0; j < grid_[i].size(); j++) {
                if (grid_[i][j] == BOX) {
                    box_pos_.push_back({ i,j });
                }
                else if (grid_[i][j] == PLAYER)
                    player_pos_ = { i,j };
                else if (grid_[i][j] == TARGET)
                    target_pos_.push_back({ i,j });

            }
            width_ = max(width_, (int)grid_[i].size());
        }
    }
}

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
        _update_objects(player_pos_, direction);
        if (is_finished())
        {
            draw();
            cout << "YOU WIN! Total steps(exculude invalid steps): " << steps_ << "." << endl;
            exit(0);
        }
    }
}
void ConsoleGame::draw() {
    for (int i = 0; i < grid_.size(); i++, cout << endl)
        for (int j = 0; j < grid_[i].size(); j++)
            cout << grid_[i][j];
}
